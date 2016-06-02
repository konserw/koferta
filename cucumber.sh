#!/bin/sh
set -evx
export QT_STEP_DELAY=10
build/kofertaSteps &
#cucumber features 
cucumber features/e2e.feature 
evince testOffer.pdf
