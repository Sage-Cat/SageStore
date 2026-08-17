package web

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"net/url"
	"strings"
	"testing"
)

func TestPostUserRegisterUsesUsersEndpointAndHashesPassword(t *testing.T) {
	originalBaseURL := storeServerBaseURL
	t.Cleanup(func() { storeServerBaseURL = originalBaseURL })

	requestReceived := make(chan User, 1)
	upstream := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if r.URL.Path != "/api/users/users" {
			t.Errorf("unexpected upstream path: %s", r.URL.Path)
		}
		if r.Method != http.MethodPost {
			t.Errorf("unexpected upstream method: %s", r.Method)
		}

		var user User
		if err := json.NewDecoder(r.Body).Decode(&user); err != nil {
			t.Errorf("decode upstream body: %v", err)
		}
		requestReceived <- user
		_, _ = w.Write([]byte(`{"result":["ok"]}`))
	}))
	defer upstream.Close()
	storeServerBaseURL = upstream.URL

	form := url.Values{"username": {"demo"}, "password": {"sensitive"}}
	request := httptest.NewRequest(http.MethodPost, "/register", strings.NewReader(form.Encode()))
	request.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	response := httptest.NewRecorder()

	PostUserRegister(response, request)

	if response.Code != http.StatusOK {
		t.Fatalf("unexpected response status: %d", response.Code)
	}
	user := <-requestReceived
	if len(user.Username) != 1 || user.Username[0] != "demo" {
		t.Fatalf("unexpected username payload: %#v", user.Username)
	}
	expectedHash := sha256.Sum256([]byte("sensitive"))
	if len(user.Password) != 1 || user.Password[0] != hex.EncodeToString(expectedHash[:]) {
		t.Fatalf("password was not hashed as expected: %#v", user.Password)
	}
}

func TestPostUserLoginReturnsBadGatewayWhenServerIsUnavailable(t *testing.T) {
	originalBaseURL := storeServerBaseURL
	t.Cleanup(func() { storeServerBaseURL = originalBaseURL })
	storeServerBaseURL = "http://127.0.0.1:1"

	form := url.Values{"username": {"demo"}, "password": {"sensitive"}}
	request := httptest.NewRequest(http.MethodPost, "/login", strings.NewReader(form.Encode()))
	request.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	response := httptest.NewRecorder()

	PostUserLogin(response, request)

	if response.Code != http.StatusBadGateway {
		t.Fatalf("unexpected response status: %d", response.Code)
	}
}
