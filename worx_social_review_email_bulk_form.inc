<?php

/**
 * A form for bulk submitting email address to send review request emails to.
 */
function worx_social_review_email_bulk_form() {
  // Find out if required defaults are set.
  $defaults_set = variable_get('worx_social_review_bulk_subject', '');
  if ($defaults_set != '') {
    $defaults_set = variable_get('worx_social_review_default_sender', '');
    if ($defaults_set != '') {
      $defaults_set = TRUE;
    }
    else {
      $defaults_set = FALSE;
    }
  }
  else {
    $defaults_set = FALSE;
  }

  if ($defaults_set) {
    $form['emails'] = array(
      '#type' => 'textarea',
      '#title' => 'Email Addresses',
      '#description' => 'Enter an email address per line',
      '#default_value' => '',
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  }
  else {
    $form['error'] = array(
      '#markup' => 'You have not filled out the default sender and/or subject line. Please fill these defaults out <a href="/admin/config/worx_features/social">here</a>.',
    );
  }

  return $form;
}

function worx_social_review_email_bulk_form_submit($form, &$form_state) {
  $emails = explode("\r\n", $form_state['values']['emails']);
  // Strip out extra whitespace, found on stack social
  // http://stackoverflow.com/questions/11576416/remove-from-string-with-php
  $i = 0;
  while ($i < count($emails)) {
    $emails[$i] = preg_replace( '/\s+/', ' ', $emails[$i]);
    $i++;
  }
  $valid = worx_social_review_email_validation($emails);
  foreach ($valid as $email => $status) {
    if ($status) {
      $exists = db_select('worx_social_review_queue', 'rq')
        ->fields('rq', array('email'))
        ->condition('email', $email, '=')
        ->execute()
        ->fetchField();

      if (!$exists) {
        db_insert('worx_social_review_queue')
          ->fields(array(
            'email' => $email,
          ))
          ->execute();
      }
      else {
        drupal_set_message(t("Email address %em is already in the Queue", array('%em' => $email)), 'warning');
      }
    }
    else {
      drupal_set_message(t("Invalid email address: %em", array('%em' => $email)), 'warning');
    }
  }
  drupal_set_message(t('Emails added to the Queue.'), 'notice');
}

/**
 * Validate email addresses.
 *
 * @param $emails
 *   An array of email addresses.
 */
function worx_social_review_email_validation(array $emails) {
  // Must include the class I'm going to be calling.
  require_once(drupal_get_path('module', 'worx_social_review') . '/smtpvalidateclass.inc');

  // Get our from email.
  $from = variable_get('email_from', '');

  // Now initiate all the stuff
  $validate = new SMTP_validateEmail();
  $validate->SMTP_validateEmail($emails, FALSE);
  $valid = $validate->validate(FALSE, $from);

 return $valid;
}
