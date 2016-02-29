<?php

/**
 * @file
 * Batch API functions, callbacks.
 */

/**
 * Batch process callback.
 *
 * @param $total
 *  Approximate total count of users in database.
 * @param $context
 */
function user_revision_table_init_data_batch_process($total, &$context) {
  // Count of rows that will be processed per iteration.
  $limit = USER_REVISION_BATCH_USERS_LIMIT;

  // Batch default values.
  if (empty($context['sandbox']['processed'])) {
    $context['sandbox']['max'] = $total ;
    $context['sandbox']['processed'] = 0;
  }

  // Fetches $limit rows from {users} table that haven't
  // already were added to {user_revision} table.
  $query = db_select('users', 'u');
  $query->leftJoin('user_revision', 'ur', 'u.uid = ur.uid');
  $query->fields('u', array('uid', 'vid', 'name', 'mail', 'theme', 'signature', 'signature_format',
    'status', 'timezone', 'language', 'picture', 'data'))
    ->condition('u.uid', 0, '!=')
    ->isNull('ur.uid')
    ->orderBy('u.uid')
    ->range(0, $limit);
  $data = $query->execute()->fetchAllAssoc('uid');

  if (!empty($data)) {
    // Build query to insert data.
    $query = db_insert('user_revision')
      ->fields(array('uid', 'vid', 'log', 'timestamp', 'authorid', 'name', 'mail', 'theme', 'signature',
        'signature_format', 'status', 'timezone', 'language', 'picture', 'data'));

    foreach ($data as $row) {
      $query->values(array(
        'uid' => $row->uid,
        'vid' => $row->vid,
        'log' => '',
        'timestamp' => REQUEST_TIME,
        'authorid' => $row->uid,
        'name' => $row->name,
        'mail' => $row->mail,
        'theme' => $row->theme,
        'signature' => $row->signature,
        'signature_format' => $row->signature_format,
        'status' => $row->status,
        'timezone' => $row->timezone,
        'language' => $row->language,
        'picture' => $row->picture,
        'data' => $row->data,
      ));
    }

    $query->execute();

    // Updates Batch message, sandbox and result data.
    $context['sandbox']['processed'] += count($data);
    $context['results']['processed'] = $context['sandbox']['processed'];
    $context['message'] = t('Processed %processed rows of %count.',
      array('%processed' => $context['sandbox']['processed'], '%count' => $context['sandbox']['max']));
    $context['finished'] = 0;
  }
  else {
    $context['finished'] = 1;
  }
}

/**
 * Batch finished callback.
 *
 * @param $success
 * @param $results
 */
function user_revision_table_init_data_batch_finished($success, $results) {
  if ($success) {
    $message = format_plural($results['processed'], 'One record was added to {user_revision} table.', '@count records were added to {user_revision} table.');
    drupal_set_message($message);
  }
  else {
    drupal_set_message( t('Finished with an error.'), 'error');
  }
}
