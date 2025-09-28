#!/bin/bash

if [[ -f '$1' ]]; then
	rm $1
fi

if [[ -n "$2" && $2 == "true" ]]; then
	sanitize="-fsanitize=address"
fi

gcc $1.c \
	-o ./$1 \
	$sanitize

if [[ $? == 0 ]]; then
	./$1
fi
