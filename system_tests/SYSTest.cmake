message("########### SYSTEM TESTS STARTED")

# TODO check python, vagrant and ansible are available
# otherwise display error and exit system test

set(repo "https://github.com/ScreamingUdder/ansible-kafka-centos.git")
set(dir "ansible-kafka-centos")

# Clone the repository with ansible and vagrant scripts in
execute_process(COMMAND "${GIT_EXECUTABLE}" clone "${repo}" "${dir}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/system_tests")

# TODO copy over any different config scripts

# Start up the virtual Kafka cluster
message("########### Starting up virtual Kafka cluster...")
set(vagrant_cmd "vagrant")
set(vagrant_args "up --provision")
execute_process(COMMAND ${vagrant_cmd} ${vagrant_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/system_tests/${dir}
        RESULT_VARIABLE rv)
message("rv='${rv}'")

message("########### Virtual Kafka cluster running")

# TODO Do the system tests (execute python script?)
set(python_cmd "python")

execute_process(COMMAND ${vagrant_cmd} ${vagrant_args}
        RESULT_VARIABLE rv)

# Shutdown the virtual cluster
set(vagrant_args "halt")
execute_process(COMMAND ${vagrant_cmd} ${vagrant_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/system_tests/ansible-kafka-centos
        RESULT_VARIABLE rv)
message("rv='${rv}'")
message("########### Virtual Kafka cluster halted")

message("########### SYSTEM TESTS COMPLETE")
