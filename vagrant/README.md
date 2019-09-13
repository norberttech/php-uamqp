# Vagrant

Before running `vagrant up` create your own `Vagrantfile` from `Vagrantfile.dist`. 

If you want to run also functional tests please update following lines in your `Vagrantfile`

```
  ansible.raw_arguments = [
    "--extra-vars='{
        \"php_version\":\"7.3.9\",
        \"uamqp_host\": \"changeme\",
        \"uamqp_policy\": \"changeme\",
        \"uamqp_key\": \"changeme",
        \"uamqp_destination\": \"changeme\",
        \"test_skip_functional": 1
    }'"
  ]
```

Replace all `changeme` with credentials, you can use [Azure Service Bus](https://azure.microsoft.com/en-us/services/service-bus/) for 
example. Do not forget to change: 

```diff
-\"test_skip_functional": 1
+\"test_skip_functional": 0
```

Otherwise functional tests are going to be skipped.

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

