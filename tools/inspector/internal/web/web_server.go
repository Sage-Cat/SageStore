package web

import (
	"net/http"

	"github.com/go-chi/chi/v5"
)

type WebServer struct {
	addr    string
	handler *chi.Mux
}

func NewWebServer(addr string, handler chi.Mux) *WebServer {
	return &WebServer{addr: addr, handler: &handler}
}

func (s *WebServer) Start() error {
	http.HandleFunc("/", http.NotFound) // Define route handler
	return http.ListenAndServe(s.addr, s.handler)
}
