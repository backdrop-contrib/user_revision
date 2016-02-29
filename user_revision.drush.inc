<?php

/**
 * Implements hook_sql_drush_sql_sync_sanitize.
 *
 * Sanitize email addresses in the user_revision table when the user_revision
 * module is enabled. Without this entity_load() will load user objects with
 * non-sanitized emails, which can lead to accidentally emailing real emails
 * in a development environment AFTER thinking you're safe running
 * `drush sql-sanitize`.
 */
function user_revision_drush_sql_sync_sanitize($site) {
  $site_settings = drush_sitealias_get_record($site);
  $databases = sitealias_get_databases_from_record($site_settings);
  if (drush_get_option('db-prefix') || !empty($databases['default']['default']['prefix'])) {
    $wrap_table_name = TRUE;
  }
  else {
    $wrap_table_name = FALSE;
  }
  $user_revision_table_updates = array();
  $message_list = array();

  // Sanitize email addresses.
  $newemail = drush_get_option(array('sanitize-email', 'destination-sanitize-email'), 'user+%uid@localhost.localdomain');
  if ($newemail != 'no' && $newemail !== 0) {
    if (strpos($newemail, '%') !== FALSE) {
      // We need a different sanitization query for Postgres and Mysql.

      $db_driver = $databases['default']['default']['driver'];
      if ($db_driver == 'pgsql') {
        $email_map = array('%uid' => "' || u.uid || '", '%mail' => "' || replace(mail, '@', '_') || '", '%name' => "' || replace(name, ' ', '_') || '");
        $newmail =  "'" . str_replace(array_keys($email_map), array_values($email_map), $newemail) . "'";
      }
      elseif ($db_driver == 'mssql') {
        $email_map = array('%uid' => "' + u.uid + '", '%mail' => "' + replace(mail, '@', '_') + '", '%name' => "' + replace(name, ' ', '_') + '");
        $newmail =  "'" . str_replace(array_keys($email_map), array_values($email_map), $newemail) . "'";
      }
      else {
        $email_map = array('%uid' => "', u.uid, '", '%mail' => "', replace(mail, '@', '_'), '", '%name' => "', replace(name, ' ', '_'), '");
        $newmail =  "concat('" . str_replace(array_keys($email_map), array_values($email_map), $newemail) . "')";
      }
      $user_revision_table_updates[] = "mail = $newmail";
    }
    else {
      $user_revision_table_updates[] = "mail = '$newemail";
    }
    $message_list[] = 'user revision email addresses';
  }

  if (!empty($user_revision_table_updates)) {
    $table = 'user_revision';
    $user_table = 'users';
    if ($wrap_table_name) {
      $table = "{{$table}}";
      $user_table = "{{$user_table}}";
    }
    $sanitize_query = "
      UPDATE {$table} ur, {$user_table} u
      SET ur." . implode(', ', $user_revision_table_updates) . "
      WHERE u.uid > 0;";
    drush_sql_register_post_sync_op('user-revision-email', dt('Reset !message in !table table', array('!message' => implode(' and ', $message_list), '!table' => $table)), $sanitize_query);
  }

}
