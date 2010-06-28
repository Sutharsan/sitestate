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
function sitestate_edit_state(&$form_state, $sitestate = array()) {
  $sitestate->settings = sitestate_settings_load($sitestate->ssid);
  //dsm($sitestate);
  
  foreach ($sitestate->settings as $setting) {
    if ($setting->type == 'module') {
      $state_modules[$setting->name] = $setting->value;
    }  
  }

  $form['ssid'] = array('#type' => 'value', '#value' => $sitestate->ssid);

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('State name'), 
    '#default_value' => $sitestate->title,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'), 
    '#default_value' => $sitestate->description,
  );

  $form['modules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Modules'), 
    '#weight' => 1, 
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
    '#tree' => true,
  );
  
  foreach ($state_modules as $module => $value) {
    $form['modules']['defined'][$module] = array(
      'name' => array(
        '#type' => 'textfield', 
        '#maxlength' => 60, 
        '#autocomplete_path' => 'admin/settings/sitestate/modules', 
        '#default_value' => $module, 
      ),
      'value' => array(
        '#type' => 'checkbox',
        '#title' => t('Enabled'),
        '#default_value' => $value,
        '#prefix' => '<div>',
        '#suffix' => '</div>',
      ),
      'current' => array(
        '#type' => 'checkbox',
        '#title' => t('Current module state'),
        '#default_value' => module_exists($module),
        '#prefix' => '<div>',
        '#suffix' => '</div>',
        '#disabled' => true,
      ),
    );
  
  }
  $form['modules']['more'] = array(
    '#prefix' => '<div><b>'. t('Add more modules') .'</b>',
    '#suffix' => '</div>',
  );
  for ( $i = 0; $i < 3; $i++) {
    $form['modules']['more'][$i] = array(
      'name' => array(
        '#type' => 'textfield', 
        '#maxlength' => 60, 
        '#autocomplete_path' => 'admin/settings/sitestate/modules', 
        '#default_value' => '',
        '#weight' => 1,
      ),
      'value' => array(
        '#title' => t('Enabled'),
        '#type' => 'checkbox',
        '#default_value' => 0,
        '#weight' => 2,
      ),
    );
  }
  
  $form['config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'), 
    '#weight' => 2, 
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $form['config']['temp'] = array('#value' => '... edit site state configurations here ...');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 10,
  );
  //dsm($form);
  return $form;
}

/**
 * Validator for the site state edit form.
 */
function sitestate_edit_state_validate($form, &$form_state) {
}

/**
 * Submit handler for the site state edit form.
 */
function sitestate_edit_state_submit($form, &$form_state) {
  //dsm($form_state);
  // Save general site state data.
  $sitestate['ssid'] = $form_state['values']['ssid'];
  $sitestate['title'] = $form_state['values']['title'];
  $sitestate['description'] = $form_state['values']['description'];
  drupal_write_record('sitestate', $sitestate, 'ssid');
  
  foreach ($form_state['values']['modules']['defined'] as $module) {
    unset($module['current']);
    $module['ssid'] = $form_state['values']['ssid'];
    $module['type'] = 'module';
    //dsm($module);
    drupal_write_record('sitestate_settings', $module, array('ssid', 'type', 'name'));
  }
  foreach ($form_state['values']['modules']['more'] as $module) {
    if ($module['name']) {
      $module['ssid'] = $form_state['values']['ssid'];
      $module['type'] = 'module';
      //dsm($module);
      drupal_write_record('sitestate_settings', $module);
    }
  }
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
function sitestate_apply_state($ssid) {
  $settings = sitestate_get_settings($ssid, TRUE);
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

  // Override off-line state if required.
  if (variable_get('sitestate_override_site_offline', SITESTATE_NO_OVERRIDE_OFFLINE) != SITESTATE_NO_OVERRIDE_OFFLINE) {
    variable_set('site_offline', variable_get('sitestate_override_site_offline', SITESTATE_NO_OVERRIDE_OFFLINE));
  }
  
  // Enable and disable modules.
  // @TODO: implement batch api for enabling/disabling modules.
  // @todo Model this after the Module page submit handling to correctly handle dependencies.
  module_enable($enable);
  module_disable($disable);
  drupal_flush_all_caches();
}

/**
 * Menu callback; Retrieve a JSON object containing autocomplete suggestions for existing modules.
 */
function sitestate_module_autocomplete($string = '') {
  $matches = array();
  if ($string) {
    $result = db_query_range("SELECT name FROM {system} WHERE type = 'module' AND LOWER(name) LIKE LOWER('%s%%')", $string, 0, 10);
    while ($user = db_fetch_object($result)) {
      $matches[$user->name] = check_plain($user->name);
    }
  }

  drupal_json($matches);
}