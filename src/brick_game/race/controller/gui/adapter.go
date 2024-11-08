package main

// #include <stdlib.h>
// #include "../../../../common/common.h"
import "C"
import (
	race "brick_game/brick_game/race/logic"
	"brick_game/common"
	"unsafe"
)

// Глобальный объект игры, чтобы держать состояние
var gameInstance *race.Race

// Конструктор для запуска игры
//
//export StartGame
func StartGame() {
	gameInstance = &race.Race{}
	gameInstance.Start()
}

// Экспортируем функцию для обработки пользовательского ввода
//
//export HandleUserInput
func HandleUserInput(action C.UserAction_t) {
	if gameInstance == nil {
		return // Если игра не запущена, ничего не делаем
	}
	gameInstance.UserInput(common.UserAction(action))
}

func ArrayToPtr(arr [][]int64) **C.int {
	result := C.create_matrix(C.FIELD_H, C.FIELD_W)
	for i := int64(0); i < common.FieldH; i++ {
		if i >= int64(len(arr)) { // Проверка границ массива
			break
		}
		rowPtr := *(**C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(result)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		for j := int64(0); j < common.FieldW; j++ {
			if j >= int64(len(arr[i])) { // Проверка границ массива
				break
			}
			elementPtr := (*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(rowPtr)) + uintptr(j)*unsafe.Sizeof(C.int(0))))
			*elementPtr = C.int(arr[i][j])
		}
	}
	return result
}

// Экспортируем функцию для обновления состояния игры
//
//export UpdateGame
func UpdateGame() C.GameInfo_t {
	gameInstance.UpdateCurrentState()
	current := gameInstance.GenerateGameInfo()
	return C.GameInfo_t{
		field:      ArrayToPtr(current.Field),
		next:       ArrayToPtr(current.Next),
		score:      C.int(current.Score),
		high_score: C.int(current.HighScore),
		level:      C.int(current.Level),
		speed:      C.int(current.Speed),
		pause:      C.int(current.Pause),
	}
}

func main() {}