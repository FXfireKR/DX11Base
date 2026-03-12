# Block System Architecture

이 문서는 현재 엔진의 **Block / State / Model / Tag / Sound 시스템의 책임과 관계**를 정리한 문서이다.
목표는 구조 혼란 방지, 중복 데이터 제거, 런타임 책임 분리이다.

---

# 1. 전체 개념 구조

```
BLOCK_ID
   |
   v
BLOCK_DEF ---- stateSource ----> BLOCK_STATE_DB
   |                                 |
   | soundProfile                     | state resolve
   v                                 v
BLOCK_SOUND_PROFILE               MODEL_ID
                                       |
                                       v
                                  BakedModel

BLOCK_TAG_DB  ---> membership ---> BLOCK_ID
```

---

# 2. 구성 요소 정의

## 2.1 BLOCK_ID

블럭 종류 자체를 식별하는 **런타임 고유 키**

예:

* minecraft:stone
* minecraft:dirt
* minecraft:oak_log

의미:

> “이 블럭은 무엇인가?”

특징:

* registry 기반 생성
* 런타임에서 모든 블럭 참조의 기준 키
* chunk 내부 BlockCell이 보유

---

## 2.2 BLOCK_DEF

블럭의 **정적 성질(static definition)**

포함 책임:

* 물성

  * air
  * opaque
  * solid
  * fullCube
* render layer
* collision type
* hardness / blastResistance
* stateSource (blockstate 파일 선택 키)
* soundProfile (사운드 그룹 키)

의미:

> “이 블럭은 어떤 성질을 가지는가?”

관계:

```
BLOCK_ID → BLOCK_DEF   (1 : 1)
```

주의:

* parent / optional 정보는 **build 단계에서만 의미 있음**
* 런타임 BLOCK_DEF는 **완전히 resolve된 상태여야 한다**

---

## 2.3 BLOCK_STATE

블럭 상태 정의 시스템

역할:

* blockstate JSON 기반 variant / multipart 해석
* 상태값 → 모델 결과 매핑

예:

* log axis=x/y/z
* slab type=top/bottom
* torch facing=east

의미:

> “같은 블럭이라도 현재 어떤 상태인가?”

관계:

```
BLOCK_DEF.stateSource → BLOCK_STATE_DB
```

---

## 2.4 MODEL_ID

렌더 가능한 **최종 모델 식별자**

중요:

* BLOCK_ID가 직접 MODEL_ID를 가지면 위험함
* 모델은 상태 결과에 따라 달라질 수 있음

권장 흐름:

```
BLOCK_ID + stateIndex
    → BLOCK_STATE resolve
        → MODEL_ID
            → BakedModel
```

의미:

> “그래서 실제로 무엇을 그려야 하는가?”

---

## 2.5 BLOCK_SOUND_PROFILE

블럭 사운드 그룹 정의

예 profile:

* stone
* wood
* grass
* glass
* sand

각 profile은 이벤트 묶음을 대표:

* break
* place
* step
* hit
* fall

관계:

```
BLOCK_DEF.soundProfile → BLOCK_SOUND_PROFILE_DB
```

의미:

> “이 블럭은 어떤 소리를 사용하는가?”

---

## 2.6 BLOCK_TAG

블럭 집합 / 규칙 분류 시스템

중요 특징:

* 블럭 정의의 일부가 아님
* 별도 DB로 관리해야 함

태그 파일 구조:

```
values:
  - minecraft:stone
  - minecraft:cobblestone
  - #minecraft:logs
```

즉:

* 개별 블럭 포함 가능
* 다른 태그 참조 가능
* resolve 후 최종 block set 생성

관계:

```
BLOCK_TAG_DB
    → resolve
        → set<BLOCK_ID>
```

런타임 조회:

```
HasTag(blockID, tagName)
```

의미:

> “이 블럭은 어떤 규칙 집합에 속하는가?”

---

# 3. 책임 분리 원칙

## BLOCK_DEF 가 가져야 하는 것

* 물성
* collision
* render layer
* hardness
* sound profile
* stateSource

## BLOCK_DEF 에 넣으면 안 되는 것

* mineable/pickaxe 같은 채굴 분류
* logs / leaves 같은 집합 규칙
* 다른 블럭들과의 관계 정보

이것들은 전부 **BLOCK_TAG_DB 책임**

---

# 4. Runtime Build Pipeline

```
Raw Layer
--------------------------------
BlockDefRaw
BlockStateRaw
ModelRaw
BlockTagRaw
SoundRaw

Build Layer
--------------------------------
BlockDefRaw → BLOCK_DEF
StateRaw → BLOCK_STATE_DB
ModelRaw → MODEL_DB
TagRaw → BLOCK_TAG_DB
SoundRaw → SOUND_PROFILE_DB

Runtime Layer
--------------------------------
BLOCK_ID
BLOCK_DEF
BLOCK_STATE_DB
MODEL_DB
BLOCK_TAG_DB
SOUND_PROFILE_DB
```

Raw 데이터는 build 완료 후 **폐기 가능**

---

# 5. 채굴 규칙 개념 분리

Minecraft 기준 최소 개념:

* CanBreak
* IsPreferredTool
* CanHarvestDrops

즉:

* 부술 수 있음
* 빠르게 캘 수 있음
* 정상 드롭 획득 가능

이 세 개는 반드시 분리해야 한다.

---

# 6. 핵심 개념 요약

```
BLOCK_ID           = 누구인가
BLOCK_DEF          = 어떤 성질인가
BLOCK_STATE        = 지금 어떤 상태인가
MODEL_ID           = 무엇을 그릴 것인가
BLOCK_SOUND_PROFILE = 어떤 소리를 쓰는가
BLOCK_TAG          = 어떤 집합 규칙에 속하는가
```

이 질문들이 서로 다르기 때문에
구조도 반드시 분리되어야 한다.

---
