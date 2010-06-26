<?php
// $Id$

/**
 * @file
 * Admin page callbacks for the site state module.
 */

/**
 * Site states list page.
 */
function sitestate_admin_page() {
  $sitestates = sitestate_load_multiple();
  
  $header = array(t('Name'), t('Description'), array('data' => t('Operations'), 'colspan' => '2'));
  $rows = array();

  foreach ($sitestates as $sitestate) {
    $row = array();
    
    // Set the name.
    $row[] = array('data' => $sitestate->title);
    
    // Set the description.
    $row[] = array('data' => $sitestate->description);  
    
    // Set the edit link.
    $row[] = array('data' => l(t('edit'), 'admin/settings/sitestate/'. $sitestate->ssid .'/edit'));
    
    // Set the delete link.
    $row[] = array('data' => l(t('delete'), 'admin/settings/sitestate/'. $sitestate->ssid .'/delete'));
    
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No site states available. <a href="@link">Define a site state</a>.', array('@link' => url('admin/settings/sitestate/add'))), 'colspan' => '4', 'class' => 'message'));
  }

  return theme('table', $header, $rows);
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
