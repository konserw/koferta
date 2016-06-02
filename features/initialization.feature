# language: en
Feature: GUI initalization
  Test the initial status of the application and proper GUI initialization

  Scenario: default state
    Given I have just turned on the application
    Then the offer table should be disabled

  Scenario: Initialization after new offer creation
    Given I have just turned on the application
    And I have created new offer
    Then the offer table should be enabled

  Scenario: Initialization after loading offer
    Given I have just turned on the application
    And I have loaded new offer
    Then the offer table should be enabled
