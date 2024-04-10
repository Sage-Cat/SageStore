package web

import (
    "fmt"
    "net/http"
    "inspector/internal/api"
)

func handleIndex(w http.ResponseWriter, r *http.Request) {
    client := api.NewClient("http://127.0.0.1:8000")  // Store IP and port
    data, err := client.FetchDebugInfo()
    if err != nil {
        http.Error(w, "Failed to fetch data", http.StatusInternalServerError)
        return
    }
    fmt.Fprintf(w, "<html><head><link rel='stylesheet' href='/css/style.css'></head><body>%s</body></html>", data)
}
