# language: en
Feature: End to end test
  Test typical user actions end to end

  Scenario: Create new offer
    Given I have just turned on the application
    And I have created new offer
    And I have added 1 x 1000 - FEOVRM110E
    And I have added 1 x 360 - FSIV040T3X1900
    And I have added 6 x 666 - VKDLV038E
    And I have choosen customer Insbud-Rybnik - Insbud-Rybnik
    And I have choosen some terms
    Then the offer printout should contain all of it
