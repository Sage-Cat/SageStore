package web

import (
    "net/http"
)

type WebServer struct {
    addr string
}

func NewWebServer(addr string) *WebServer {
    return &WebServer{addr: addr}
}

func (s *WebServer) Start() error {
    http.HandleFunc("/", handleIndex)  // Define route handler
    return http.ListenAndServe(s.addr, nil)
}
