#!/usr/bin/env bash

third_party/gn/out/gn gen out
ninja -C out
patchelf --replace-needed libktube_lib.so "$PWD"/third_party/ktube/out/libktube_lib.so out/klytics
patchelf --replace-needed libcpr.so.1 "$PWD"/third_party/cpr/lib/libcpr.so out/ut_api
patchelf --replace-needed libcpr.so.1 "$PWD"/third_party/cpr/lib/libcpr.so out/ut_klytics
