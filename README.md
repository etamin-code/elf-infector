# elf-infector
проект з курсу операційні системи: elf infector і детектор для нього

склад команди: Олександр Кухар і Данило Кальченко

## work principle
### of injector
program finds codecave in executable load segment of the target file, inject payload bytecode there, change entrypoint of target file to start of the injected payload, in the end of payload jumps to original entrypoint.
### of detector
program goes to entrypoint of target file end compare first bytes of ep with first bytes of payload, if they are the same return 1, 0 otherwise
### of deinjector
goes to the end of payload, take the adress of original ep, and replace injected ep with original one.

## usage
git clone https://github.com/etamin-code/elf-infector
cd elf-infector
mkdir build
bash scripts/build_sources_script

bash scripts/detect_script elfs_for_testing/ build/payload #check the correct work
./elfs_for_testing/hello

bash scripts/inject_script elfs_for_testing/ build/payload

bash scripts/detect_script elfs_for_testing/ build/payload #check the work after injecting
./elfs_for_testing/hello

bash scripts/deinject_script elfs_for_testing/ build/payload #check the work after deinjecting
bash scripts/detect_script elfs_for_testing/ build/payload
./elfs_for_testing/hello

## example of usage
![image](https://user-images.githubusercontent.com/70692373/187034890-f5c484fc-77e7-46b7-808e-0ed0f04dee05.png)

![image](https://user-images.githubusercontent.com/70692373/187034900-f0e82646-c76e-4a13-a211-12d2299ab2cd.png)

![image](https://user-images.githubusercontent.com/70692373/187034917-176626d0-8378-4a70-a820-e59b311b17ce.png)
![image](https://user-images.githubusercontent.com/70692373/187034927-9bde2bc6-47db-43e4-b80c-00ced55681a3.png)

![image](https://user-images.githubusercontent.com/70692373/187034935-d2cd5e19-e46e-4d1c-8aa6-ca0e482f658c.png)


![image](https://user-images.githubusercontent.com/70692373/187034945-a583787e-98fd-4323-aecd-fdafd4324476.png)
![image](https://user-images.githubusercontent.com/70692373/187034950-20be6400-3fbd-4f81-a80c-3c8e2fd9b98e.png)


## example of injecting of large directories
![image](https://user-images.githubusercontent.com/70692373/187035164-01b8d136-c995-4105-8ba8-1e5656bf4cf1.png)

