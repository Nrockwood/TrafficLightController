# CppAppTemplate

## Getting Started

### Add SSH Keys
Generate new public & private key pair. Copy public key from $HOME/.ssh/id_rsa.pub and paste into new SSH key in Github Account Settings.
```bash
ssh-keygen -t rsa -b 4096
```

### Clone Repo
```bash
git clone --recursive git@github.com:Nrockwood/CppAppTemplate.git
```

## Tests
Run the tests with
```bash
make build test
```

## Dev
To see a list of commonly used tasks you may use during development of this project run
```bash
make
```