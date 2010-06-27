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
  $sitestates = sitestate_load_all();
  
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

/**
 * Apply site state.
 */
function sitestate_apply_state($sitestate) {
  $settings = sitestate_get_settings($sitestate);
  $enable = array();
  $disable = array();
  foreach ($settings as $setting) {
    switch ($setting->type) {
    
      case 'variable':
        variable_set($setting->name, $setting->value);
        break;
        
      case 'module':
        if (module_exists($setting->name) != $setting->value) {
          if ($setting->value == 1) {
            $enable[] = $setting->name;
          }
          else {
            $disable[] = $setting->name;
          }
        }
        break;
        
    }
  }
  // Enable and disable modules.
  // @TODO: implement batch api for enabling/disabling modules.
  // @todo Model this after the Module page submit handling to correctly handle dependencies.
  module_enable($enable);
  module_disable($disable);
  drupal_flush_all_caches();
}