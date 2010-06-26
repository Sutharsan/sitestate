<?php
// $Id$

/**
 * @file
 * Admin page callbacks for the site state module.
 */

/**
 * Form builder; Site state list page.
 */
function sitestate_admin_state(&$form_state, $sitestate) {
  $form['temp'] = array('#value' => '... administer site states here ...');
  return $form;
}

/**
 * Form builder; Site state add/edit page.
 */
function sitestate_edit_state($sitestate = array()) {
  $form['temp'] = array('#value' => '... edit site states here ...');
  return $form;
}

/**
 * Form builder; Site state delete page.
 */
function sitestate_delete_state($sitestate) {
  $form['temp'] = array('#value' => '... confirm delete site states here ...');
  return $form;
}
