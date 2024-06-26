package web

import (
	"crypto/sha256"
	"encoding/hex"
)

type User struct {
	Username []string `json:"username"`
	Password []string `json:"password"`
}

func (u *User) HashPasswords() {
	for i, password := range u.Password {

		hash := sha256.New()
		hash.Write([]byte(password))

		hashed := hash.Sum(nil)

		u.Password[i] = hex.EncodeToString(hashed)
	}
}
