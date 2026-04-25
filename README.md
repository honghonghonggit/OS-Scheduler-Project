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
- **주요 파일**: `Proportional-Share/` 내 관련 소스 (직접 구현)
- **지원 알고리즘**: Lottery, Stride Scheduling

---

##  Directory Structure

- **Priority-Based/**: FCFS, SPN, RR, MLFQ 로직이 포함된 `sched.cc` 수록
- **Proportional-Share/**: Lottery, Stride 로직이 포함된 `sched.cc` 수록
- **Makefile**: 빌드 및 컴파일 자동화
