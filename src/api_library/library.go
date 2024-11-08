package brick_lib

import (
	"brick_game/common"
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

var serverAddr string

type LibError struct {
	Message string
	Code    int
}

type GamesList struct {
	Games []GameInfo `json:"games"`
}

type GameInfo struct {
	Name string `json:"string"`
	Id   int    `json:"id"`
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

type UserAction struct {
	ActionId int  `json:"action_id"`
	Hold     bool `json:"hold"`
}

type ErrorMessage struct {
	Message string `json:"message"`
}

func (e *LibError) Error() string {
	if e.Code != -1 {
		return fmt.Sprintf("server returned error code: %d. %s", e.Code, e.Message)
	} else {
		return fmt.Sprintf("%s", e.Message)
	}
}

func SetServerAddr(addr string) {
	serverAddr = addr
}

func GetGamesList() (GamesList, error) {
	err := getServerSetError()
	if err != nil {
		return GamesList{}, err
	}
	resp, err := http.Get(fmt.Sprintf("%s/games", serverAddr))
	if err != nil {
		return GamesList{}, err
	}
	defer resp.Body.Close()

	var list GamesList
	err = json.NewDecoder(resp.Body).Decode(&list)
	if err != nil {
		return GamesList{}, err
	}
	return list, nil
}

func SelectGame(id int) error {
	err := getServerSetError()
	if err != nil {
		return err
	}
	req, err := http.NewRequest("POST", fmt.Sprintf("%s/games/%d", serverAddr, id), bytes.NewBuffer([]byte("{}")))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Connection", "close")
	if err != nil {
		return err
	}
	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	if resp.StatusCode != 200 {
		return &LibError{
			Message: "responce code != 200",
			Code:    resp.StatusCode,
		}
	}
	return nil
}

func SendAction(action common.UserAction) error {
	err := getServerSetError()
	if err != nil {
		return err
	}
	actionToServ := UserAction{ActionId: int(action), Hold: false}
	payload, err := json.Marshal(actionToServ)
	if err != nil {
		return err
	}
	req, err := http.NewRequest("POST", fmt.Sprintf("%s/actions", serverAddr), bytes.NewBuffer(payload))
	req.Header.Set("Content-Type", "application/json")
	req.Header.Set("Connection", "close")
	if err != nil {
		return err
	}
	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	if resp.StatusCode != 200 {
		body, _ := io.ReadAll(resp.Body)
		fmt.Println(string(body))
		return &LibError{
			Message: "responce code != 200",
			Code:    resp.StatusCode,
		}
	}
	return nil
}

func GetState() (common.GameInfo, error) {
	err := getServerSetError()
	if err != nil {
		return common.GameInfo{}, err
	}
	resp, err := http.Get(fmt.Sprintf("%s/state", serverAddr))
	if err != nil {
		return common.GameInfo{}, err
	}
	defer resp.Body.Close()
	var gameState GameState
	err = json.NewDecoder(resp.Body).Decode(&gameState)
	if err != nil {
		return common.GameInfo{}, err
	}
	pause := 0
	if gameState.Pause {
		pause = 1
	}
	return common.GameInfo{
		Field:     boolBoolToInt64(gameState.Field),
		Next:      boolBoolToInt64(gameState.Next),
		Score:     int64(gameState.Score),
		HighScore: int64(gameState.HighScore),
		Level:     int64(gameState.Level),
		Speed:     int64(gameState.Speed),
		Pause:     int64(pause),
	}, nil
}

func boolBoolToInt64(bools [][]bool) [][]int64 {
	var ints = make([][]int64, len(bools))
	for i, b := range bools {
		ints[i] = boolToInt64(b)
	}
	return ints
}

func boolToInt64(bools []bool) []int64 {
	var ints = make([]int64, len(bools))
	for i, b := range bools {
		if b {
			ints[i] = 1
		} else {
			ints[i] = 0
		}
	}
	return ints
}

func getServerSetError() error {
	if serverAddr == "" {
		return &LibError{
			Message: "you did not set server address. use SetServerAddr",
			Code:    -1,
		}
	}
	return nil
}
