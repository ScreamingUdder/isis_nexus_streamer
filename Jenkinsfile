rpm_script='''
rm -rf package/*
mkdir -p package/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

cp build/isis_nexus_streamer-0.1.0.tar.gz package/SOURCES/
cp code/isis_nexus_streamer.spec package/SPECS/

rpmbuild \
    --define "_topdir $(pwd)/package" \
    --define "_version 0.1.0" \
    --define "_release $BUILD_NUMBER" \
    -bb package/SPECS/isis_nexus_streamer.spec
'''

node('kafka-client') {
    dir("code") {
        stage("Checkout") {
            checkout scm
        }
    }

    dir("build") {
        stage("CMake") {
            sh "cmake --version"
            sh "HDF5_ROOT=$HDF5_ROOT \
                cmake ../code -DLibRDKafka_ROOT_DIR=$DM_ROOT"
        }

        stage("Build") {
            sh "make VERBOSE=1"
        }

        stage("Unit Tests") {
            sh "./unitTests ../code/data/ --gtest_output=xml:test_results.xml"
        }

        stage("clang-format") {
            sh "make VERBOSE=1 check_clang_format"
        }

        stage("cppcheck") {
            sh "make cppcheck"
        }

        stage("Docs") {
            sh "make doc"
        }

        stage("Package") {
            sh "make package"
        }
    }

    stage("RPM") {
        sh(rpm_script)
    }
}
