<?php
/**
 * @file
 * Form functions for the Election module.
 */

/**
 * Form builder function for adding or editing an election.
 */
function election_form($form, &$form_state, stdClass $election) {

  $form['#id'] = 'election-form';
  $form['#election'] = $election;

  $form_state['election'] = $election;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($election->title) ? $election->title : '',
    '#required' => TRUE,
  );

  $date_format = variable_get('date_format_short', 'Y-m-d H:i');
  if (module_exists('date_popup')) {
    $date_widget = 'date_popup';
  }
  else {
    $date_widget = 'date_text';
  }

  if (!isset($election->type_info)) {
    $election->type_info = _election_type_get_info($election->type);
  }
  if (!empty($election->type_info['has candidates'])) {

    $nopen_default = isset($election->nopen_time) ? $election->nopen_time : NULL;
    $nclose_default = isset($election->nclose_time) ? $election->nclose_time : NULL;
    $form['nstatus'] = array(
      '#type' => 'select',
      '#title' => t('Nominations status'),
      '#default_value' => isset($election->nstatus) ? $election->nstatus : ELECTION_STATUS_CLOSED,
      '#options' => array(
        ELECTION_STATUS_CLOSED => t('Closed'),
        ELECTION_STATUS_OPEN => t('Open'),
        ELECTION_STATUS_SCHEDULED => t('Scheduled'),
      ),
    );
    $form['nschedule'] = array(
      '#title' => t('Nominations schedule'),
      '#type' => 'fieldset',
      '#description' => 'Set the opening and closing times for nominations.',
      '#states' => array(
        'visible' => array(
          ':input[name=nstatus]' => array('value' => (string) ELECTION_STATUS_SCHEDULED),
        ),
      ),
    );
    $form['nschedule']['nopen_time'] = array(
      '#title' => t('Opening time'),
      '#type' => $date_widget,
      '#default_value' => $nopen_default ? date('Y-m-d H:i:s', $nopen_default) : NULL,
      '#date_format' => $date_format,
      '#date_increment' => 5,
      '#date_year_range' => '-2:+5',
    );
    $form['nschedule']['nclose_time'] = array(
      '#title' => t('Closing time'),
      '#type' => $date_widget,
      '#default_value' => $nclose_default ? date('Y-m-d H:i:s', $nclose_default) : NULL,
      '#date_format' => $date_format,
      '#date_increment' => 5,
      '#date_year_range' => '-2:+5',
    );
  }

  $vopen_default = isset($election->vopen_time) ? $election->vopen_time : NULL;
  $vclose_default = isset($election->vclose_time) ? $election->vclose_time : NULL;

  $form['vstatus'] = array(
    '#type' => 'select',
    '#title' => t('Voting status'),
    '#default_value' => isset($election->vstatus) ? $election->vstatus : ELECTION_STATUS_CLOSED,
    '#options' => array(
      ELECTION_STATUS_CLOSED => t('Closed'),
      ELECTION_STATUS_OPEN => t('Open'),
      ELECTION_STATUS_SCHEDULED => t('Scheduled'),
    ),
  );
  $form['vschedule'] = array(
    '#title' => t('Voting schedule'),
    '#type' => 'fieldset',
    '#description' => 'Set the opening and closing times for voting.',
    '#states' => array(
      'visible' => array(
        ':input[name=vstatus]' => array('value' => (string) ELECTION_STATUS_SCHEDULED),
      ),
    ),
  );
  $form['vschedule']['vopen_time'] = array(
    '#title' => t('Opening time'),
    '#type' => $date_widget,
    '#default_value' => $vopen_default ? date('Y-m-d H:i:s', $vopen_default) : NULL,
    '#date_format' => $date_format,
    '#date_increment' => 5,
    '#date_year_range' => '-2:+5',
  );
  $form['vschedule']['vclose_time'] = array(
    '#title' => t('Closing time'),
    '#type' => $date_widget,
    '#default_value' => $vclose_default ? date('Y-m-d H:i:s', $vclose_default) : NULL,
    '#date_format' => $date_format,
    '#date_increment' => 5,
    '#date_year_range' => '-2:+5',
  );

  $form['published'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => isset($election->published) ? $election->published : 0,
    '#weight' => 99,
  );

  $form['buttons'] = array(
    '#type' => 'actions',
    '#weight' => 100,
  );
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('election_form_submit'),
  );

  if (!empty($election->election_id)) {
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('election_form_delete_submit'),
      '#access' => entity_access('delete', 'election', $election),
    );
  }

  $form['#validate'][] = 'election_form_validate';

  // Add custom fields from the Field system.
  field_attach_form('election', $election, $form, $form_state);

  return $form;

}

/**
 * Form validate function for the form election_form.
 */
function election_form_validate($form, &$form_state) {

  // VALIDATE VOTING SCHEDULE...
  $vstatus = $form_state['values']['vstatus'];
  $vopen_time = $form_state['values']['vopen_time'];
  if (is_array($vopen_time)) {
    $vopen_time = $vopen_time['date'] . ' ' . ($vopen_time['time'] ? '00:00' : $vopen_time['time']);
  }
  $vclose_time = $form_state['values']['vclose_time'];
  if (is_array($vclose_time)) {
    $vclose_time = $vclose_time['date'] . ' ' . ($vclose_time['time'] ? '00:00' : $vclose_time['time']);
  }
  // Check that the opening and closing times are set if voting is scheduled.
  if ($vstatus == ELECTION_STATUS_SCHEDULED
      && (empty($vopen_time) || empty($vclose_time))) {
    form_set_error('vschedule', t('You must set the opening and closing times when the voting status is Scheduled.'));
  }
  // Check that the closing time is after the opening time.
  elseif ($vstatus == ELECTION_STATUS_SCHEDULED
        && strtotime($vclose_time) <= strtotime($vopen_time)) {
    form_set_error('vschedule', t('The closing time of voting must be after the opening time.'));
  }

  // VALIDATE NOMINATIONS SCHEDULE...
  if (isset($form_state['values']['nstatus'])) {
    $nstatus = $form_state['values']['nstatus'];
    $nopen_time = $form_state['values']['nopen_time'];
    if (is_array($nopen_time)) {
      $nopen_time = $nopen_time['date'] . ' ' . ($nopen_time['time'] ? '00:00' : $nopen_time['time']);
    }
    $nclose_time = $form_state['values']['nclose_time'];
    if (is_array($nclose_time)) {
      $nclose_time = $nclose_time['date'] . ' ' . ($nclose_time['time'] ? '00:00' : $nclose_time['time']);
    }
    // Check that nominations and voting are not open at the same time.
    if ($nstatus == ELECTION_STATUS_OPEN && $vstatus == ELECTION_STATUS_OPEN) {
      form_set_error('vstatus', t('Voting should not be open at the same time as nominations.'));
    }
    // Check that the opening and closing times are set, if relevant.
    elseif ($nstatus == ELECTION_STATUS_SCHEDULED
        && (empty($nopen_time) || empty($nclose_time))) {
      form_set_error('nschedule', t('You must set the opening and closing times when the nominations status is Scheduled.'));
    }
    // Check that the closing time is after the opening time.
    elseif ($nstatus == ELECTION_STATUS_SCHEDULED
          && strtotime($nclose_time) <= strtotime($nopen_time)) {
      form_set_error('nschedule', t('The closing time of nominations must be after the opening time.'));
    }
    // Check that the voting time is after the nominations close.
    elseif (@$vstatus == ELECTION_STATUS_SCHEDULED
          && strtotime($vopen_time) < strtotime($nclose_time)) {
      form_set_error('nschedule', t('Nominations can only be scheduled so that they close before the start of voting.'));
    }
  }

  // Check other Fields.
  field_attach_form_validate('election', $form_state['election'], $form, $form_state);

}

/**
 * Form submit function for the Save button in the form election_form.
 */
function election_form_submit($form, &$form_state) {

  global $user;

  $election = &$form_state['election'];

  if (empty($election->uid)) {
    $election->uid = $user->uid;
  }

  foreach ($election as $property => $value) {
    if (isset($form_state['values'][$property])) {
      $election->$property = $form_state['values'][$property];
    }
  }

  field_attach_submit('election', $election, $form, $form_state);

  // Save the election.
  election_save($election);

  // Notify the user that the election was saved.
  $type = _election_type_get_info($election->type);
  drupal_set_message(
    t('The @type %title was saved.', array('@type' => $type['name'], '%title' => $election->title))
  );

  if (empty($_GET['destination'])) {
    $form_state['redirect'] = election_uri_path($election);
  }

}

/**
 * Form submit function for the Delete button in the form election_form.
 */
function election_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $election = $form['#election'];
  $form_state['redirect'] = array(
    election_uri_path($election) . '/delete',
    array('query' => $destination),
  );
}

/**
 * Form builder function to confirm the delete action for an election.
 *
 * @see confirm_form()
 */
function election_form_delete_confirm($form, &$form_state, stdClass $election) {

  drupal_set_breadcrumb(
    _election_build_breadcrumb($election)
  );

  $form['#election'] = $election;
  $form['election_id'] = array(
    '#type' => 'value',
    '#value' => $election->election_id,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete %title?', array('%title' => $election->title)),
    election_uri_path($election),
    t('Deleting an election also deletes ALL associated data, for example votes. This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit callback, called after confirming delete for an election.
 */
function election_form_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $election = election_load($form_state['values']['election_id']);
    $delete = election_delete($form_state['values']['election_id']);
    if ($delete) {
      watchdog('election', 'Deleted election "%title" (type: @type)', array('@type' => $election->type, '%title' => $election->title));
    }

    $type = _election_type_get_info($election->type);

    if ($delete) {
      drupal_set_message(
        t('The @type %title has been deleted.', array('@type' => $type['name'], '%title' => $election->title))
      );
    }
    else {
      drupal_set_message(
        t('Failed to delete the @type %title.', array('@type' => $type['name'], '%title' => $election->title)),
        'error'
      );
    }

  }
  if (empty($_GET['destination'])) {
    $form_state['redirect'] = 'elections';
  }
}