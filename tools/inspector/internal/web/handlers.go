package web

import (
	"bytes"
	"encoding/json"
	"fmt"
	"html/template"
	"inspector/templates"
	"io"
	"log"
	"net/http"
)

var Body = []byte{}

func PostUserLogin(w http.ResponseWriter, r *http.Request) {

	var UrlString string = "http://localhost:8001/api/users/login"
	r.ParseForm()
	formData := make(map[string][]string)
	for key, values := range r.Form {
		formData[key] = values
	}
	NewUser := User{
		Username: formData["username"],
		Password: formData["password"],
	}
	NewUser.HashPasswords()

	jsonData, err := json.Marshal(NewUser)
	if err != nil {
		fmt.Println("JSON marshal err-:", err)
		return
	}

	if r.Method == http.MethodPost {
		req, err := http.NewRequest("POST", UrlString, bytes.NewBuffer(jsonData))
		if err != nil {
			log.Println(err)
		}

		req.Header.Set("Content-type", "application/json")

		cli := &http.Client{}
		res, err := cli.Do(req)
		if err != nil {
			log.Println(err)
		}

		log.Println(res.Status)
		log.Println(res.Header)
		log.Println(NewUser)

		Body, _ = io.ReadAll(res.Body)
		defer res.Body.Close()
		t, err := template.ParseFS(templates.FS, "loginpage.html")
		if err != nil {
			panic(err)
		}

		t.Execute(w, string(Body))
	}
	if r.Method == http.MethodGet {
		t, err := template.ParseFS(templates.FS, "loginpage.html")
		if err != nil {
			panic(err)
		}
		t.Execute(w, string(Body))
	}
}

func PostUserRegister(w http.ResponseWriter, r *http.Request) {

	var UrlString string = "http://localhost:8001/api/users/register"

	r.ParseForm()

	formData := make(map[string][]string)

	for key, values := range r.Form {
		formData[key] = values
	}
	NewUser := User{
		Username: formData["username"],
		Password: formData["password"],
	}
	NewUser.HashPasswords()

	jsonData, err := json.Marshal(NewUser)
	if err != nil {
		fmt.Println("JSON marshal err-:", err)
		return
	}
	if r.Method == http.MethodPost {
		req, err := http.NewRequest("POST", UrlString, bytes.NewBuffer(jsonData))
		if err != nil {
			log.Println(err)
		}

		req.Header.Set("Content-type", "application/json")

		cli := &http.Client{}
		res, err := cli.Do(req)
		if err != nil {
			log.Println(err)
		}

		defer res.Body.Close()

		log.Println(res.Status)
		log.Println(NewUser)
		log.Println(res.Header)

		Body, _ = io.ReadAll(res.Body)
	}

	t, err := template.ParseFS(templates.FS, "loginpage.html")
	if err != nil {
		panic(err)
	}

	t.Execute(w, string(Body))

}
