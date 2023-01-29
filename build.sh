#!/usr/bin/env fish

set fish_trace on

podman run \
#  --interactive \
#  --tty \
  --rm \
  --userns=keep-id \
  --volume (pwd):(pwd):Z \
  joshwyant/gcc-cross \
  /usr/bin/bash -c "cd "(pwd)" && ./build-impl.sh"
#  /usr/bin/bash -c "cd "(pwd)" && bash -i"
