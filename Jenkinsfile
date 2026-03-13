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
                    set -eux
                    rm -rf build || true
                '''
            }
        }
        stage('Conan Install') {
            steps {
                sh '''
                    set -eux
                    conan install . --output-folder=build --build=missing
                '''
            }
        }
        stage('Docs') {
            steps {
                sh '''
                    set -eux
                    python3 build.py docs
                '''
            }
        }
        stage('CMake Configure') {
            steps {
                sh '''
                    set -eux
                    mkdir -p build
                    cd build
                    cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_TESTS=ON
                '''
            }
        }
        stage('CMake Build') {
            steps {
                sh '''
                    set -eux
                    cd build
                    cmake --build . --parallel
                '''
            }
        }
        stage('Run Tests') {
            steps {
                sh '''
                    set -eux
                    cd build
                    ctest --output-on-failure --verbose
                '''
            }
        }
        stage('Inventory Smoke') {
            steps {
                sh '''
                    set -eux
                    bash scripts/smoke/fullstack_inventory_smoke.sh
                '''
            }
        }
        stage('GUI Startup Smoke') {
            steps {
                sh '''
                    set -eux
                    bash scripts/smoke/fullstack_gui_startup_smoke.sh
                '''
            }
        }
    }
    post {
        success {
            archiveArtifacts artifacts: 'build/_server/SageStoreServer,build/_server/scripts/create_db.sql,build/_client/SageStoreClient,docs/Deployment_Runbook.md', fingerprint: true
        }
        always {
            sh 'killall Xvfb || true'
        }
    }
}
