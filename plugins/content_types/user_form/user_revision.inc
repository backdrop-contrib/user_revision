<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_user.png',
  'title' => t('User revision control'),
  'description' => t('Fields to control creating user revision.'),
  'required context' => new ctools_context_required(t('Form'), 'user_edit_form'),
  'category' => t('Form'),
);

function user_revision_user_revision_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('user_revision');

  $block->title = '';
  $block->delta = 'revision_information';

  if (isset($context->form)) {
    $block->content = array();
    $block->content['revision_information'] = isset($context->form['revision_information']) ? $context->form['revision_information'] : NULL;
    unset($context->form['revision_information']);
  }
  else {
    $block->content = t('User revision.');
  }
  return $block;
}

function user_revision_user_revision_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" user revision', array('@s' => $context->identifier));
}

function user_revision_user_revision_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
