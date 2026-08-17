package main

import (
	"inspector/internal/web"
	"log"

	"github.com/go-chi/chi/v5"
)

func main() {

	r := chi.NewRouter()

	r.Get("/", web.PostUserRegister)
	//r.Get("/login", web.PostUserLogin)

	r.Post("/register", web.PostUserRegister)
	r.Post("/login", web.PostUserLogin)
	log.Println("Inspector listening on http://127.0.0.1:8081")

	webServer := web.NewWebServer("127.0.0.1:8081", *r)
	if err := webServer.Start(); err != nil {
		log.Fatalf("Failed to start web server: %s", err)
	}

}
