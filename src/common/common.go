package common

// #include "common.h"
import "C"
import "unsafe"

const FieldH int64 = int64(C.FIELD_H)
const FieldW int64 = int64(C.FIELD_W)
const ActionButton int64 = int64(C.ACTION_BUTTON)
const TerminateButton int64 = int64(C.TERMINATE_BUTTON)
const PauseButton int64 = int64(C.PAUSE_BUTTON)
const StartButton int64 = int64(C.START_BUTTON)

type UserAction int64

const (
	Start UserAction = iota
	Pause
	Terminate
	Left
	Right
	Up
	Down
	Action
	Nothing
)

type Banner int64

const (
	None Banner = iota
	GameStart
	GamePause
)

type GameInfo struct {
	Field     [][]int64
	Next      [][]int64
	Score     int64
	HighScore int64
	Level     int64
	Speed     int64
	Pause     int64
}

func GameInfoCToGo(CGameInfo C.GameInfo_t) GameInfo {
	return GameInfo{
		Field:     PtrToArray(CGameInfo.field),
		Next:      PtrToArray(CGameInfo.next),
		Score:     int64(CGameInfo.score),
		HighScore: int64(CGameInfo.high_score),
		Level:     int64(CGameInfo.level),
		Speed:     int64(CGameInfo.speed),
		Pause:     int64(CGameInfo.pause),
	}
}

func PtrToArray(cPtr **C.int) [][]int64 {
	result := make([][]int64, FieldH)

	for i := int64(0); i < FieldH; i++ {
		result[i] = make([]int64, FieldW)
		rowPtr := *(**C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(cPtr)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		for j := int64(0); j < FieldW; j++ {
			value := *(*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(rowPtr)) + uintptr(j)*unsafe.Sizeof(C.int(0))))
			result[i][j] = int64(value)
		}
	}
	return result
}

func ArrayToPtr(arr [][]int64) **C.int {
	result := C.create_matrix(C.FIELD_H, C.FIELD_W)
	for i := int64(0); i < FieldH; i++ {
		rowPtr := *(**C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(result)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		for j := int64(0); j < FieldW; j++ {
			elementPtr := (*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(rowPtr)) + uintptr(j)*unsafe.Sizeof(C.int(0))))
			*elementPtr = C.int(arr[i][j])
		}
	}
	return result
}
