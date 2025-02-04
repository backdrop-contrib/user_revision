<?php

/**
 * @file
 * Themeable function callbacks for user_rev_diff.module.
 */

/**
 * Display inline diff metadata.
 */
function theme_user_rev_diff_inline_metadata($vars) {
  backdrop_add_css(backdrop_get_path('module', 'diff') . '/diff.css');
  $user = $vars['user'];

  $output = "<div class='diff-inline-metadata clear-block'>";
  $output .= "<div class='diff-inline-byline'>";
  $output .= t('Updated by !name on @date', array(
    '!name' => theme('username', array('account' => $user)),
    '@date' => format_date($user->revision_timestamp, 'small'),
  ));
  $output .= "</div>";
  $output .= "<div class='diff-inline-legend clear-block'>";
  $output .= "<label>" . t('Legend') . "</label>";
  $output .= theme('diff_inline_chunk', array('text' => t('Added'), 'type' => 'add'));
  $output .= theme('diff_inline_chunk', array('text' => t('Changed'), 'type' => 'change'));
  $output .= theme('diff_inline_chunk', array('text' => t('Deleted'), 'type' => 'relete'));
  $output .= "</div>";
  $output .= "</div>";
  return $output;
}
