#!/usr/bin/env fish

podman run \
  --interactive \
  --tty \
  --rm \
  --userns=keep-id \
  --volume (pwd):(pwd):Z \
  joshwyant/gcc-cross \
  /usr/bin/bash -c "cd "(pwd)" && bash -i"
