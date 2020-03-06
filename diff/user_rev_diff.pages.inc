<?php

/**
 * @file
 * Menu callbacks for hook_menu().
 */

/**
 * Menu callback - show latest diff for a given node.
 */
function user_rev_diff_latest($user) {
  $revisions = user_revision_list($user);
  $new = array_shift($revisions);
  $old = array_shift($revisions);
  drupal_goto(_user_revision_base_path($user) . "/diff/{$old->vid}/{$new->vid}");
}

/**
 * Generate an overview table of older revisions of a node and provide
 * an input form to select two revisions for a comparison.
 */
function user_rev_diff_diffs_overview($user) {
  drupal_set_title(t('Revisions for @title', array('@title' => $user->name)));
  return drupal_get_form('user_rev_diff_user_revisions', $user);
}

/**
 * Input form to select two revisions.
 *
 * @param $viewed_user
 *   User whose revisions are displayed for selection.
 */
function user_rev_diff_user_revisions($form, $form_state, $viewed_user) {

  $data = user_revision_list_build($viewed_user);

  // Each element of $data is an array containing:
  // 'row': an array keyed by the revision-id, containing an array keyed 'data' containing markup.
  // 'operations: an array containing revert and delete links.

  foreach ($data as $item) {
    $row = $item['row'];
    $vid = key($row);
    $revision_ids[$vid] = '';
    $form['info'][$vid] = array(
      '#markup' => $row[$vid]['data'],
      '#revision' => $row[$vid]['revision'],
    );
    $operations = array();
    foreach ($item['operations'] as $operation) {
      if (!is_array($operation)) {
        $operations[] = array('#markup' => $operation);
      }
    }
    $form['operations'][$vid] = $operations;
  }
  $new_vid = key($revision_ids);
  next($revision_ids);
  $old_vid = key($revision_ids);
  $form['diff']['old'] = array(
    '#type' => 'radios',
    '#options' => $revision_ids,
    '#default_value' => $old_vid
  );
  $form['diff']['new'] = array(
    '#type' => 'radios',
    '#options' => $revision_ids,
    '#default_value' => $new_vid
  );
  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => $viewed_user->uid,
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Compare'));

  // use the diff module's theme function.
  $form['#attached'] = diff_build_attachments(TRUE);
  $form['#theme'] = 'diff_node_revisions';

  return $form;
}

/**
 * Submit code for input form to select two revisions.
 */
function user_rev_diff_user_revisions_submit($form, &$form_state) {
  // the ids are ordered so the old revision is always on the left
  $old_vid = min($form_state['values']['old'], $form_state['values']['new']);
  $new_vid = max($form_state['values']['old'], $form_state['values']['new']);
  $uid = $form_state['values']['uid'];
  $form_state['redirect'] =  _user_revision_base_path($uid) . "/diff/$old_vid/$new_vid";
}

/**
 * Validation for input form to select two revisions.
 */
function user_rev_diff_user_revisions_validate($form, &$form_state) {
  $old_vid = $form_state['values']['old'];
  $new_vid = $form_state['values']['new'];
  if ($old_vid==$new_vid || !$old_vid || !$new_vid) {
    form_set_error('diff', t('Select different revisions to compare.'));
  }
}

/**
 * Create output string for a comparison of 'user' between
 * versions 'old_vid' and 'new_vid'.
 *
 * @param $user
 *   User on which to perform comparison
 * @param $old_vid
 *   Version ID of the old revision.
 * @param $new_vid
 *   Version ID of the new revision.
 */
function user_rev_diff_diffs_show($user, $old_vid, $new_vid) {
  module_load_include('inc', 'diff', 'diff.pages');
  // Set same title as on the 'Revisions' tab for consistency
  drupal_set_title(t('Revisions for @name', array('@name' => $user->name)));

  $user_revisions = user_revision_list($user);
  $old_user = user_revision_load($user->uid, $old_vid);
  $new_user = user_revision_load($user->uid, $new_vid);
  $link_base = _user_revision_base_path($user);
  // Generate table header (date, username, logmessage).
  $old_header = t('!date by !username', array(
    '!date' => l(format_date($old_user->revision_timestamp),
    "$link_base/view/{$old_user->vid}"),
    '!username' => theme('username', array('account' => $user_revisions[$old_vid])),
  ));
  $new_header = t('!date by !username', array(
    '!date' => l(format_date($new_user->revision_timestamp),
    "$link_base/view/{$new_user->vid}"),
    '!username' => theme('username', array('account' => $user_revisions[$new_vid])),
  ));

  $old_log = $old_user->log != '' ? '<p class="revision-log">'. filter_xss($old_user->log) .'</p>' : '';
  $new_log = $new_user->log != '' ? '<p class="revision-log">'. filter_xss($new_user->log) .'</p>' : '';

  // Generate previous diff/next diff links.
  $next_vid = _diff_get_next_vid($user_revisions, $new_vid);
  if ($next_vid) {
    $next_link = l(t('next diff >'), "$link_base/diff/$new_vid/$next_vid");
  }
  else {
    $next_link = '';
  }
  $prev_vid = _diff_get_previous_vid($user_revisions, $old_vid);
  if ($prev_vid) {
    $prev_link = l(t('< previous diff'), "$link_base/diff/$prev_vid/$old_vid");
  }
  else {
    $prev_link = '';
  }

  $cols = _diff_default_cols();
  $header = _diff_default_header($old_header, $new_header);
  $rows = array();
  if ($old_log || $new_log) {
    $rows[] = array(
      array(
        'data' => $old_log,
        'colspan' => 2
      ),
      array(
        'data' => $new_log,
        'colspan' => 2
      )
    );
  }
  $rows[] = array(
    array(
      'data' => $prev_link,
      'class' => array('diff-prevlink'),
      'colspan' => 2
    ),
    array(
      'data' => $next_link,
      'class' => array('diff-nextlink'),
      'colspan' => 2
    )
  );
  $rows = array_merge($rows, _user_rev_diff_body_rows($old_user, $new_user));
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array('diff')),
    'cols' => $cols
  ));
  $user_view = user_view($new_user, 'diff_standard');
  $user_output = drupal_render($user_view);
  // Omit header for account display if no rendered output.
  if (!empty(trim(strip_tags($user_output)))) {
    if ($user->vid == $new_vid) {
      $output .= '<div class="diff-section-title">'. t('Current revision:') .'</div>';
    }
    else {
      $output .= '<div class="diff-section-title">'. t('Revision of !new_date:', array('!new_date' => format_date($new_user->revision_timestamp))) .'</div>';
    }
    $output .= $user_output;
  }
  return $output;
}

/**
 * Creates an array of rows which represent a diff between $old_user and $new_user.
 * The rows can be used via theme('diff_table') to be displayed.
 *
 * @param $old_user
 *   User for comparison which will be displayed on the left side.
 * @param $new_user
 *   User for comparison which will be displayed on the right side.
 */
function _user_rev_diff_body_rows($old_user, $new_user) {
  // make use of the user comparison functions in the diff module.
  module_load_include('inc', 'diff', 'diff.pages');
  $context = array(
    'states' => array(variable_get('diff_default_state_user', 'raw')),
    'view_mode' => 'diff_standard',
  );
  return diff_entity_body_rows('user', $old_user, $new_user, $context);
}

/**
 * Show the inline diff of a given user object v. the user with $old_vid.
 * If $old_vid = 0 returns the normally rendered content of the user object.
 * If metadata is TRUE a header will be added with a legend explaining the color code.
 * This function returns HTML code.
 */
function user_rev_diff_inline_show($user, $old_vid = 0, $metadata = TRUE) {
  $new_user = /*clone*/ $user;
  user_build_content($new_user);
  $new = drupal_render($new_user->content);
  module_load_include('inc', 'diff', 'diff.pages');
  if ($old_vid) {
    $old_user = user_revision_load($user->uid, $old_vid, TRUE);
    user_build_content($old_user);
    $old = drupal_render($old_user->content);
    $output = $metadata ? theme('user_rev_diff_inline_metadata', array('user' => $new_user)) : '';
    $output .= diff_get_inline($old, $new);
    return $output;
  }
  return $new;
}
