package go_controller

import (
	"brick_game/common"
	"time"
)

type Controller interface {
	Init()
	GetInput() (common.UserAction, error)
}

type Model interface {
	UpdateCurrentState()
	UserInput(action common.UserAction)
	GetGameInfo() common.GameInfo
}

type View interface {
	Render(current common.GameInfo)
}

func Start(m Model, v View, c Controller) {
	c.Init()
	for {
		for i := 0; i < 10; i++ {
			action, err := c.GetInput()
			if err == nil {
				m.UserInput(action)
			}
			v.Render(m.GetGameInfo())
			time.Sleep(time.Millisecond * time.Duration(m.GetGameInfo().Speed) / 10)
		}

		m.UpdateCurrentState()
		v.Render(m.GetGameInfo())

	}

}
