pipeline {
    agent any
    environment {
        PATH = "/var/lib/jenkins/.local/bin:${env.PATH}"
        LD_LIBRARY_PATH = "/var/lib/jenkins/.conan2/p/icu4ea64724a76de/p/lib:/var/lib/jenkins/.conan2/p/md4cbe5ef51cc4864/p/lib:${env.LD_LIBRARY_PATH}"
        QT_QPA_PLATFORM = "offscreen" 
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
                    cmake .. --preset conan-release -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_TESTS=ON
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
            sh 'killall Xvfb || true'
        }
    }
}
