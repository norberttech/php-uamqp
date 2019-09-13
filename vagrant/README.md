# Vagrant

Before running `vagrant up` create your own `Vagrantfile` from `Vagrantfile.dist`. 

By default PHP version is defined in `Vagrantfile:26`, feel free to change it any time you want to build extension
against different version of php. After change you need to run `vagrant provision`. 

You can have as many compiled php version as you need in vagrant. Ansible playbook will always setup `PHP_UAMQP_PHP_EXECUTABLE`
env var to the latest compiled version.

The easiest way to compile the extension is tu launch `vagrantu up`, then login into machine using `vagrant ssh` and
execute following commands: 

```console
$ cd php-uamqp
$ bin/init.sh
$ make 
```

