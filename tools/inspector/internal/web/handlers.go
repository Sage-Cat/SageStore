package web

import (
	"bytes"
	"encoding/json"
	"html/template"
	"inspector/templates"
	"io"
	"log"
	"net/http"
	"time"
)

const maxUpstreamResponseBytes = 1 << 20

var (
	storeServerBaseURL = "http://127.0.0.1:8001"
	storeHTTPClient    = &http.Client{Timeout: 10 * time.Second}
)

func renderLoginPage(w http.ResponseWriter, body string) {
	t, err := template.ParseFS(templates.FS, "loginpage.html")
	if err != nil {
		log.Printf("parse login template: %v", err)
		http.Error(w, "Unable to render inspector page", http.StatusInternalServerError)
		return
	}

	if err := t.Execute(w, body); err != nil {
		log.Printf("render login template: %v", err)
	}
}

func forwardUserRequest(w http.ResponseWriter, r *http.Request, endpoint string) {
	if r.Method != http.MethodPost {
		renderLoginPage(w, "")
		return
	}

	if err := r.ParseForm(); err != nil {
		http.Error(w, "Invalid form data", http.StatusBadRequest)
		return
	}

	user := User{Username: r.Form["username"], Password: r.Form["password"]}
	user.HashPasswords()
	jsonData, err := json.Marshal(user)
	if err != nil {
		log.Printf("encode user request: %v", err)
		http.Error(w, "Unable to encode request", http.StatusInternalServerError)
		return
	}

	req, err := http.NewRequestWithContext(
		r.Context(), http.MethodPost, storeServerBaseURL+endpoint, bytes.NewReader(jsonData))
	if err != nil {
		log.Printf("create SageStore request: %v", err)
		http.Error(w, "Unable to create upstream request", http.StatusInternalServerError)
		return
	}
	req.Header.Set("Content-Type", "application/json")

	response, err := storeHTTPClient.Do(req)
	if err != nil {
		log.Printf("SageStore request failed: %v", err)
		http.Error(w, "SageStore server is unavailable", http.StatusBadGateway)
		return
	}
	defer response.Body.Close()

	body, err := io.ReadAll(io.LimitReader(response.Body, maxUpstreamResponseBytes))
	if err != nil {
		log.Printf("read SageStore response: %v", err)
		http.Error(w, "Unable to read upstream response", http.StatusBadGateway)
		return
	}

	renderLoginPage(w, string(body))
}

func PostUserLogin(w http.ResponseWriter, r *http.Request) {
	forwardUserRequest(w, r, "/api/users/login")
}

func PostUserRegister(w http.ResponseWriter, r *http.Request) {
	forwardUserRequest(w, r, "/api/users/users")
}
