package main

import (
	"fmt"
	"net/http"
)

func main() {
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/", fs)

	fmt.Println("Сервер запущен на порту :19999...")
	err := http.ListenAndServe(":19999", nil)
	if err != nil {
		fmt.Println("Ошибка запуска сервера:", err)
	}
}
