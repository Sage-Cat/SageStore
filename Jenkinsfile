pipeline {
    agent any
    environment {
        PATH = "/home/sagecat/Qt/6.5.3/gcc_64/bin:~/.local/bin:$PATH"
    }
    stages {
        stage('Checkout') {
            steps {
                // Get the code from the version control system.
                checkout scm
            }
        }
        stage('Conan Install') {
            steps {
                sh '''
                    conan install . --output-folder=build --build=missing
                '''
            }
        }
        stage('CMake Configure') {
            steps {
                sh '''
                    cd build
                    cmake .. --preset conan-debug
                '''
            }
        }
        stage('CMake Build') {
            steps {
                sh '''
                    cd build
                    cmake --build .
                '''
            }
        }
        stage('Run Tests') {
            steps {
                sh '''
                    cd build
                    ctest --verbose
                '''
            }
        }
    }
}
