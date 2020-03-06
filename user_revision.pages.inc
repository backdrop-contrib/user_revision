<?php

/**
 * @file
 * UI pages for revisions, copy-paste from node.pages.inc.
 */

/**
 * Generate an overview table of older revisions of a user.
 */
function user_revision_overview($viewed_user) {
  global $user;
  drupal_set_title(t('Revisions for %title', array('%title' => $viewed_user->name)), PASS_THROUGH);

  $header = array(t('Revision'), array('data' => t('Operations'), 'colspan' => 2),);
  $data = user_revision_list_build($viewed_user);

  $rows = array();
  foreach ($data as $item) {
    $rows[] = array_merge($item['row'], $item['operations']);
  }

  $build['user_revisions_table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
    '#header' => $header,
  );
  return $build;
}

/**
 * Revert a revision.
 */
function user_revision_revert_confirm($form, $form_state, $user_revision) {
  $form['#user_revision'] = $user_revision;
  return confirm_form($form, t('Are you sure you want to revert to the revision from %revision-date?', array('%revision-date' => format_date($user_revision->revision_timestamp))), _user_revision_base_path($user_revision), '', t('Revert'), t('Cancel'));
}

/**
 * Revert a revision.
 */
function user_revision_revert_confirm_submit($form, &$form_state) {
  $user_revision = $form['#user_revision'];
  $user_revision->revision = 1;
  // Removing password to not hash it again.
  $user_revision->pass = '';
  $user_revision->log = t('Copy of the revision from %date.', array('%date' => format_date($user_revision->revision_timestamp)));
  user_save($user_revision, (array) $user_revision);
  watchdog('user', 'reverted %title revision %revision.', array('%title' => $user_revision->name, '%revision' => $user_revision->vid));
  drupal_set_message(t('%title has been reverted back to the revision from %revision-date.', array('%title' => $user_revision->name, '%revision-date' => format_date($user_revision->revision_timestamp))));
  $form_state['redirect'] = _user_revision_base_path($user_revision);
}

/**
 * Delete a revision.
 */
function user_revision_delete_confirm($form, $form_state, $user_revision) {
  $form['#user_revision'] = $user_revision;
  return confirm_form($form, t('Are you sure you want to delete the revision from %revision-date?', array('%revision-date' => format_date($user_revision->revision_timestamp))), _user_revision_base_path($user_revision), t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Delete a revision.
 */
function user_revision_delete_confirm_submit($form, &$form_state) {
  $user_revision = $form['#user_revision'];
  user_revision_delete($user_revision);

  watchdog('user', 'deleted %title revision %revision.', array('%title' => $user_revision->name, '%revision' => $user_revision->vid));
  drupal_set_message(t('Revision from %revision-date %title has been deleted.', array('%revision-date' => format_date($user_revision->revision_timestamp), '%title' => $user_revision->name)));
  if (db_select('user_revision', 'ur')->condition('ur.uid', $user_revision->uid)->countQuery()->execute()->fetchField() > 1) {
    $form_state['redirect'] .= _user_revision_base_path($user_revision);
  }
  else {
    $form_state['redirect'] = "user/{$user_revision->uid}";
  }
}
