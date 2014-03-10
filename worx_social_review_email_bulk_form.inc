<?php

/**
 * A form for bulk submitting email address to send review request emails to.
 */
function worx_social_review_email_bulk_form() {
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

  return $form;
}

function worx_social_review_email_bulk_form_submit($form, &$form_state) {
  $emails = explode("\n", $form_state['input']['emails']);
  watchdog('worx_social_review_email_bulk', '<pre>' . print_r($emails, TRUE) . '</pre>');
  foreach ($emails as $email) {
    // Strip out extra whitespace, found on stack social
    // http://stackoverflow.com/questions/11576416/remove-from-string-with-php
    $email = preg_replace( '/\s+/', ' ', $email);
    $valid = check_email_address($email);
    if ($valid) {
      db_insert('worx_social_review_queue')
        ->fields(array(
          'email' => $email,
        ))
        ->execute();
    }
    else {
      drupal_set_message(t("Invalid email address"), 'warning');
    }
  }
  drupal_set_message(t('submission logged'), 'notice');
}

/**
 * Email format validation function found on
 * stackoverflow.com/questions/12026842/how-to-validate-an-email-address-in-php
 */
function check_email_address($email) {
  /**
   * the below pattern regex is
   * Copyright © Michael Rushton 2009-10
   * http://squiloople.com/
   * redistributed per http://lxr.php.net/xref/PHP_5_4/ext/filter/logical_filters.c#501
   */
  $pattern = '/^(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){255,})(?!(?:(?:\\x22?\\x5C[\\x00-\\x7E]\\x22?)|(?:\\x22?[^\\x5C\\x22]\\x22?)){65,}@)(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22))(?:\\.(?:(?:[\\x21\\x23-\\x27\\x2A\\x2B\\x2D\\x2F-\\x39\\x3D\\x3F\\x5E-\\x7E]+)|(?:\\x22(?:[\\x01-\\x08\\x0B\\x0C\\x0E-\\x1F\\x21\\x23-\\x5B\\x5D-\\x7F]|(?:\\x5C[\\x00-\\x7F]))*\\x22)))*@(?:(?:(?!.*[^.]{64,})(?:(?:(?:xn--)?[a-z0-9]+(?:-+[a-z0-9]+)*\\.){1,126}){1,}(?:(?:[a-z][a-z0-9]*)|(?:(?:xn--)[a-z0-9]+))(?:-+[a-z0-9]+)*)|(?:\\[(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){7})|(?:(?!(?:.*[a-f0-9][:\\]]){7,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,5})?)))|(?:(?:IPv6:(?:(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){5}:)|(?:(?!(?:.*[a-f0-9]:){5,})(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3})?::(?:[a-f0-9]{1,4}(?::[a-f0-9]{1,4}){0,3}:)?)))?(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))(?:\\.(?:(?:25[0-5])|(?:2[0-4][0-9])|(?:1[0-9]{2})|(?:[1-9]?[0-9]))){3}))\\]))$/iD';

  if (preg_match($pattern, $email) === 1) {
    return TRUE;
  }
  return FALSE;
}