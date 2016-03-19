# language: en
Feature: GUI initalization
  test the initial status of the application 

  Scenario: Initialization
    Given I just turned on the application
    Then the offer table should be disabled

