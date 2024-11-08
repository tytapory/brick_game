package server

/*
#cgo CXXFLAGS: -std=c++14 -I../../brick_game/snake/logic -I../../common
#cgo LDFLAGS: -L. -lsnake_adapter -lstdc++
#include "snake_adapter.h"
*/
import "C"
import (
	"brick_game/common"
	"brick_game/common/web_controller"
	"fmt"
	"time"
	"unsafe"
)

type SnakeDecorator struct {
	s *C.Snake
}

func (s SnakeDecorator) UpdateCurrentState() common.GameInfo {
	fmt.Println("tryingf to get info")
	return GameInfoCToGo(C.Snake_updateCurrentState(s.s))
}

func (s SnakeDecorator) UserInput(action common.UserAction, hold bool) {
	if C.getStatus(s.s) == 6 {
		time.Sleep(time.Second * 5)
		C.Snake_delete(s.s)
		s.s = C.Snake_new()
	} else {
		C.Snake_userInput(s.s, C.int(action), C._Bool(hold))
	}
}

func GameInfoCToGo(CGameInfo C.GameInfo_t) common.GameInfo {
	return common.GameInfo{
		Field:     PtrToArray(CGameInfo.field, int(common.FieldH), int(common.FieldW)),
		Next:      PtrToArray(CGameInfo.next, 4, 4),
		Score:     int64(CGameInfo.score),
		HighScore: int64(CGameInfo.high_score),
		Level:     int64(CGameInfo.level),
		Speed:     int64(CGameInfo.speed) / 500,
		Pause:     int64(CGameInfo.pause),
	}
}

func PtrToArray(cPtr **C.int, h, w int) [][]int64 {
	result := make([][]int64, h)

	for i := int64(0); i < int64(h); i++ {
		rowPtr := *(**C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(cPtr)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))

		// Проверяем, не является ли указатель на строку нулевым
		if rowPtr == nil {
			fmt.Printf("Row pointer at index %d is nil\n", i)
			continue // Пропускаем итерацию или обрабатываем иначе
		}

		result[i] = make([]int64, w)
		for j := int64(0); j < int64(w); j++ {
			value := *(*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(rowPtr)) + uintptr(j)*unsafe.Sizeof(C.int(0))))
			result[i][j] = int64(value)
		}
	}
	return result
}

func RunSnake() web_controller.Channels {
	snake := C.Snake_new()
	s := SnakeDecorator{s: snake}
	channels := web_controller.Run(s)
	go func(chans web_controller.Channels, snek *C.Snake) {
		stop := false
		for !stop {
			select {
			case <-chans.Done:
				fmt.Println("stopped snake")
				C.Snake_delete(snek)
				stop = true
			default:
			}
			time.Sleep(time.Millisecond * 1000)
		}
	}(channels, snake)
	return channels
}
