#!/usr/bin/env bash
find . -name '*.pyc' | xargs rm
find . -name 'tags' | xargs rm
find . -name '*.pcap' | xargs rm
rm ./res/*.flow
rm ./res/*.txt
