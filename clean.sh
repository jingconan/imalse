#!/usr/bin/env bash
find . -name '*.pyc' | xargs rm
find . -name 'tags' | xargs rm
