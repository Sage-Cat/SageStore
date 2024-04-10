package main

import (
    "log"
    "inspector/internal/web"
)

func main() {
    webServer := web.NewWebServer(":8080") 
    if err := webServer.Start(); err != nil {
        log.Fatalf("Failed to start web server: %s", err)
    }
}
