package server

import (
	"brick_game/common"
	"brick_game/common/web_controller"
	server "brick_game/server/controllers"
	"encoding/json"
	"fmt"
	"net"
	"net/http"
	"strconv"
	"strings"
)

var games = []Game{{"tetris", server.RunTetris},
	{"snake", server.RunSnake},
	{"race", server.RunRace}}

var users = make(map[string]web_controller.Channels)

type Game struct {
	Name string
	Run  func() web_controller.Channels
}

type GamesList struct {
	Games []GameInfo `json:"games"`
}

type GameInfo struct {
	Id   int    `json:"id"`
	Name string `json:"string"`
}

type UserAction struct {
	ActionId int  `json:"action_id"`
	Hold     bool `json:"hold"`
}

type GameState struct {
	Field     [][]bool `json:"field"`
	Next      [][]bool `json:"next"`
	Score     int      `json:"score"`
	HighScore int      `json:"high_score"`
	Level     int      `json:"level"`
	Speed     int      `json:"speed"`
	Pause     bool     `json:"pause"`
}

type ErrorMessage struct {
	Message string `json:"message"`
}

func gamesList(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	var responce GamesList
	for i := range games {
		responce.Games = append(responce.Games, GameInfo{i, games[i].Name})
	}
	json.NewEncoder(w).Encode(responce)
}

func runGame(w http.ResponseWriter, r *http.Request) {
	ip, _, _ := net.SplitHostPort(r.RemoteAddr)
	fmt.Println("Running game for IP:")
	fmt.Println(ip)
	w.Header().Set("Content-Type", "application/json")
	if isPlaying(ip) {
		w.WriteHeader(http.StatusConflict)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Пользователь уже запустил другую игру"})
		return
	}

	path := r.URL.Path

	parts := strings.Split(path, "/")

	if len(parts) != 3 || parts[1] != "games" {
		w.WriteHeader(http.StatusNotFound)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Неверный путь"})
		return
	}

	idStr := parts[2]
	id, err := strconv.Atoi(idStr)

	if err != nil {
		w.WriteHeader(http.StatusNotFound)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Неверный формат id"})
		return
	}

	if id < 0 || id > len(games)-1 {
		w.WriteHeader(http.StatusNotFound)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Игры с этим айди не существует"})
		return
	}

	gameChannels := games[id].Run()
	users[ip] = gameChannels
	w.WriteHeader(http.StatusOK)
}

func executeAction(w http.ResponseWriter, r *http.Request) {

	w.Header().Set("Content-Type", "application/json")
	var action UserAction

	err := json.NewDecoder(r.Body).Decode(&action)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Ошибка в теле запроса"})
		return
	}
	ip, _, _ := net.SplitHostPort(r.RemoteAddr)
	if !isPlaying(ip) {
		w.WriteHeader(http.StatusBadRequest)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Игрок еще ни во что не играет"})
		return
	}
	select {
	case users[ip].Input <- web_controller.ActionHold{Id: common.UserAction(action.ActionId), Hold: action.Hold}:
	default:
	}
	w.WriteHeader(http.StatusOK)
}

func state(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	ip, _, _ := net.SplitHostPort(r.RemoteAddr)
	if !isPlaying(ip) {
		w.WriteHeader(http.StatusBadRequest)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Игра не запущена"})
		return
	}
	var state common.GameInfo
	select {
	case state = <-users[ip].Output:
	default:
		w.WriteHeader(http.StatusInternalServerError)
		json.NewEncoder(w).Encode(ErrorMessage{Message: "Нет кадра"})
		return
	}
	pause := false
	if state.Pause != 0 {
		pause = true
	}
	w.WriteHeader(http.StatusOK)
	fmt.Println(state.Field)
	json.NewEncoder(w).Encode(GameState{
		Field:     intToBoolMatrix(state.Field, int(common.FieldH), int(common.FieldW)),
		Next:      intToBoolMatrix(state.Next, 4, 4),
		Score:     int(state.Score),
		HighScore: int(state.HighScore),
		Level:     int(state.Level),
		Speed:     int(state.Speed),
		Pause:     pause,
	})
}

func intToBoolMatrix(matrix [][]int64, h, w int) [][]bool {
	result := make([][]bool, h)
	for i := 0; i < h; i++ {
		result[i] = make([]bool, w)
	}
	for i := 0; i < h; i++ {
		for j := 0; j < w; j++ {
			if matrix[i][j] != 0 {
				result[i][j] = true
			} else {
				result[i][j] = false
			}
		}
	}
	return result
}

func isPlaying(ip string) bool {
	var result bool
	if _, exists := users[ip]; exists {
		select {
		case <-users[ip].Done:
			fmt.Printf("Game is not running for %s\n", ip)
			delete(users, ip)
			result = false
		default:
			result = true
		}
	}
	return result
}

func handleActions(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodOptions {
		return
	}
	w.Write([]byte("Action received"))
}

func enableCORS(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		// Установите заголовки CORS
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PUT, DELETE")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization")

		// Обработка preflight-запросов
		if r.Method == http.MethodOptions {
			w.WriteHeader(http.StatusOK)
			return
		}

		next.ServeHTTP(w, r)
	})
}

func Run() {
	http.HandleFunc("/games/", runGame)
	http.HandleFunc("/games", gamesList)
	http.HandleFunc("/actions", executeAction)
	http.HandleFunc("/state", state)
	fmt.Println("Server starting")
	http.ListenAndServe(":8989", enableCORS(http.DefaultServeMux))
}
