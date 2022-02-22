# simpapp XATMI server and client using Casual

Example server and client using XATMI implemented by [Casual](https://bitbucket.org/casualcore/) and built with Casual's build tools.

## Clone and build

After cloning main branch the casual dependencies can be added as Git subtrees as shown below.

```sh
# add and squash-merge dependency 'casual'
REMOTE=casual
REF=feature/1.6/dev
git remote add ${REMOTE} git@bitbucket.org:casualcore/casual-make.git
git fetch ${REMOTE} ${REF}
git merge --strategy=ours --no-commit --squash --allow-unrelated-histories ${REMOTE}/${REF}
git read-tree --prefix=${REMOTE}/ -u ${REMOTE}/${REF}
git commit -m "[${REMOTE}] updated subtree from $(git show-ref ${REMOTE}/${REF})"

# add and squash-merge dependency 'casual-thirdparty'
REMOTE=casual-thirdparty
REF=feature/new-casual-make
git remote add ${REMOTE} git@bitbucket.org:casualcore/casual-make.git
git fetch ${REMOTE} ${REF}
git merge --strategy=ours --no-commit --squash --allow-unrelated-histories ${REMOTE}/${REF}
git read-tree --prefix=${REMOTE}/ -u ${REMOTE}/${REF}
git commit -m "[${REMOTE}] updated subtree from $(git show-ref ${REMOTE}/${REF})"

# add and squash-merge dependency 'casual-make'
REMOTE=casual-make
REF=feature/0.1/dev
git remote add ${REMOTE} git@bitbucket.org:casualcore/casual-make.git
git fetch ${REMOTE} ${REF}
git merge --strategy=ours --no-commit --squash --allow-unrelated-histories ${REMOTE}/${REF}
git read-tree --prefix=${REMOTE}/ -u ${REMOTE}/${REF}
git commit -m "[${REMOTE}] updated subtree from $(git show-ref ${REMOTE}/${REF})"
```

To update a subtree dependency, fetch the remote ref and commit a squash-merge.

```sh
# fetch and squash-merge updates from dependency REMOTE/REF
git fetch ${REMOTE} ${REF}
git merge --strategy=subtree --no-commit --squash --allow-unrelated-histories ${REMOTE}/${REF}
git commit -m "[${REMOTE}] updated subtree from $(git show-ref ${REMOTE}/${REF})"
```

### Build and install casual

```sh
sudo dnf install gcc-c++ libuuid-devel libcurl-devel sqlite-devel

DIR=${PWD}
export CASUAL_BUILD_HOME=${DIR}/casual
export CASUAL_THIRDPARTY=${DIR}/casual-thirdparty
export CASUAL_MAKE_HOME=${DIR}/casual-make
export PYTHONPATH=${DIR}/casual/make:${CASUAL_MAKE_HOME}
export PATH=${CASUAL_MAKE_HOME}/source:${DIR}/casual/bin:${PATH}

pushd casual
casual-make compile
casual-make link
casual-make test

# install binaries to directory ${CASUAL_HOME}
export CASUAL_HOME=${DIR}/installdir
mkdir ${CASUAL_HOME}
casual-make install
popd
```

### Build simpapp

This builds `simpapp/bin/simpserv` and `simpapp/bin/simpcl`.

Set environment variables for building. 

```sh
export PATH=${CASUAL_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${CASUAL_HOME}/lib
```

Build from `simpapp/makefile.cmk`.

```sh
pushd simpapp
casual-make compile
casual-make link
popd
```

Or, build with CLI. (The client executable must specify an entrypoint different from `main`, which is why `simpcl2.cpp` exists with entrypoint `main2`.)

```sh
pushd simpapp
# server
casual-build-server --output bin/simpserv \
  --build-directives src/simpserv.cpp \
  --server-definition server.yaml

# client
casual-build-executable --output bin/simpcl \
  --build-directives src/simpcl2.cpp \
  --definition executable.yaml
popd
```

## Run in container

```sh
ctr=$(buildah from registry.fedoraproject.org/fedora:35)

buildah run ${ctr} dnf -y install libuuid libcurl sqlite passwd

buildah run ${ctr} useradd --comment 'Casual' --create-home --groups wheel --shell /bin/bash casual
buildah run ${ctr} passwd casual

buildah config \
  --user casual:0 \
  --workingdir /home/casual/ \
  --env CASUAL_HOME=/home/casual/installdir \
  --env CASUAL_LOG_PATH=/tmp/casual.log \
  --env CASUAL_LOG='(error|warning|information)' \
  --env CASUAL_DOMAIN_HOME=/home/casual/simpapp \
  --cmd '["casual", "domain", "--boot", "/home/casual/simpapp/domain.yaml"]' \
  --entrypoint '["/home/casual/entrypoint.sh"]' \
  ${ctr}

cat <<<'#!/bin/env sh
export PATH=${CASUAL_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${CASUAL_HOME}/lib:${LD_LIBRARY_PATH}
umask 0002
exec "$@"' > entrypoint.sh

chmod ug+rwx entrypoint.sh
chmod -R ug+rwX installdir/ simpapp/{bin/,domain.yaml,domain.env}

buildah copy --chown casual:root ${ctr} entrypoint.sh /home/casual/
buildah copy --chown casual:root ${ctr} installdir/ /home/casual/installdir
buildah copy --chown casual:root ${ctr} simpapp/{bin/,domain.yaml,domain.env} /home/casual/simpapp

# run interactively
buildah run ${ctr} ./entrypoint.sh bash -il
```

Inside the container, Start the domain with the server and then run client.

```sh
casual domain --boot ${CASUAL_DOMAIN_HOME}/domain.yaml
${CASUAL_DOMAIN_HOME}/bin/simpcl hello
casual domain --shutdown
```
