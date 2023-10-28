pipeline {
    agent any
    environment {
        PATH = "/home/sagecat/Qt/6.5.3/gcc_64/bin:/var/lib/jenkins/.local/bin:${env.PATH}"
        QT_QPA_PLATFORM = "offscreen" // Set Qt to use offscreen platform
    }
    stages {
        stage('Checkout') {
            steps {
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
                    mkdir -p build
                    cd build
                    cmake .. --preset conan-release
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
    post {
        always {
            // Clean up by stopping any background processes if needed
            sh 'killall Xvfb || true'
        }
    }
}
