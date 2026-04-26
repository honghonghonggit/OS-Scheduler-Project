# 🚀 Linux CPU Scheduler Simulator

Linux 환경에서 프로세스 스케줄링의 메커니즘을 분석하고 핵심 알고리즘을 직접 구현한 프로젝트입니다.

> Core Implementation
> - 본 프로젝트는 제공된 시뮬레이터 프레임워크를 기반으로 핵심 스케줄링 로직을 개발했습니다.
> - 주요 작업물: `Priority-Based/sched.cc`, `Proportional-Share/sched.cc` 파일 내 모든 스케줄링 알고리즘 로직 구현

---

## 🛠️ Implementation Details

### 1. Priority-Based Scheduling
시스템의 우선순위와 응답성을 결정하는 기본 로직입니다.
- **주요 파일**: `Priority-Based/sched.cc` (직접 구현)
- **지원 알고리즘**: FCFS, SPN, RR, MLFQ

### 2. Proportional-Share Scheduling
자원의 공정한 배분을 위한 정밀 스케줄링 기법입니다.
- **주요 파일**: `Proportional-Share/sched.cc` 내 관련 소스 (직접 구현)
- **지원 알고리즘**: Lottery, Stride Scheduling

---

##  Directory Structure

- **Priority-Based/**: FCFS, SPN, RR, MLFQ 로직이 포함된 `sched.cc` 수록
- **Proportional-Share/**: Lottery, Stride 로직이 포함된 `sched.cc` 수록
- **Makefile**: 빌드 및 컴파일 자동화

##  Usage 
### 1. 환경 설정 (최초 1회)
Google Test 등 빌드에 필요한 의존성 라이브러리를 설치합니다.
```bash
chmod +x install_package.sh
./install_package.sh

2. 빌드 및 테스트
각 스케줄러 폴더로 이동하여 아래 명령어를 실행합니다.

# 기존 빌드 기록 삭제 후 재빌드
make clean
make

# 알고리즘 테스트 실행
./test


