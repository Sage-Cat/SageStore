pipeline {
    agent any
    environment {
        PATH = "/home/sagecat/Qt/6.5.3/gcc_64/bin:/var/lib/jenkins/.local/bin:${env.PATH}"
        QT_QPA_PLATFORM = "offscreen" // Ensures Qt uses the offscreen platform, useful for headless environments
    }
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        stage('Clean Build Folder') {
            steps {
                sh '''
                    rm -rf build || true
                '''
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
                    cmake .. --preset conan-release -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_CLIENT_TESTS=ON -DBUILD_SERVER_TESTS=ON
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
