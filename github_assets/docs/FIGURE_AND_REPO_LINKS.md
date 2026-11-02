# GitHub links for the final report (CS4433-LMS)

**Repository:** https://github.com/katlehochristophersenqhe-maker/CS4433-LMS

Replace `main` with your branch name if needed. After you upload each file, these links work for the marker.

---

## How to use in the Word/PDF report

Under every figure, write:

> **Figure X – [title]**  
> Available on GitHub:  
> https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/FILENAME.png

Do the same for the video and phase documents.

---

## Recommended folder layout on GitHub

```
CS4433-LMS/
├── report/
│   └── CS4433_Final_Report.pdf
├── docs/
│   ├── diagrams/
│   │   ├── 01_architecture.png
│   │   ├── 02_component.png
│   │   ├── 03_class_diagram.png
│   │   ├── 04_state_machine.png
│   │   ├── 05_activity.png
│   │   ├── 06_sequence_submit.png
│   │   ├── 07_sequence_moderate.png
│   │   ├── 08_erd.png
│   │   └── 09_deployment.png
│   ├── phases/
│   │   ├── Phase1_Planning.pdf
│   │   ├── Phase2_Requirements.pdf
│   │   └── Phase3_Design.pdf
│   ├── FIGURE_AND_REPO_LINKS.md
│   ├── INSTALLATION.md
│   ├── USER_MANUALS.md
│   └── ...
├── media/
│   └── presentation_15min.mp4
└── README.md
```

---

## Figure hyperlinks (paste into the report)

### Architecture / design (Phase 3)

**Figure – Layered architecture**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/01_architecture.png

**Figure – Component diagram**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/02_component.png

**Figure – Class diagram**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/03_class_diagram.png

**Figure – Submission state machine**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/04_state_machine.png

**Figure – Activity diagram (swimlanes)**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/05_activity.png

**Figure – Sequence diagram: Submit Assignment**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/06_sequence_submit.png

**Figure – Sequence diagram: Moderate and Release**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/07_sequence_moderate.png

**Figure – Entity-Relationship Diagram (3NF)**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/08_erd.png

### Deployment (Phase 7)

**Figure – UML Deployment Diagram**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/09_deployment.png

Also keep the draw.io source (optional but useful):  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/09_deployment.drawio

### Planning (Phase 1)

**Figure – Gantt chart**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/10_gantt.png

---

## Documents and media links

**Final report (PDF)**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/report/CS4433_Final_Report.pdf

**Presentation / viva video (15 min)**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/media/presentation_15min.mp4

**Phase 1 – Planning**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/phases/Phase1_Planning.pdf

**Phase 2 – Requirements**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/phases/Phase2_Requirements.pdf

**Phase 3 – Design**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/phases/Phase3_Design.pdf

**Installation guide**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/INSTALLATION.md

**User manuals**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/USER_MANUALS.md

**CI workflow**  
https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/.github/workflows/ci.yml

---

## README block (copy into README.md)

```markdown
## Assessment submission (marker)

| Item | Link |
|------|------|
| Final report | [report/CS4433_Final_Report.pdf](report/CS4433_Final_Report.pdf) |
| Presentation video | [media/presentation_15min.mp4](media/presentation_15min.mp4) |
| All diagrams | [docs/diagrams](docs/diagrams) |
| Phase documents | [docs/phases](docs/phases) |
| Installation | [docs/INSTALLATION.md](docs/INSTALLATION.md) |
| User manuals | [docs/USER_MANUALS.md](docs/USER_MANUALS.md) |
| Release v1.0 | tag `v1.0` |
| Release v1.1 (CR1) | tag `v1.1` |
```

---

## draw.io export steps (for each diagram)

1. Open the diagram in draw.io / diagrams.net  
2. **File → Export as → PNG** (zoom 100% or higher, transparent or white background)  
3. Save with the file name from the list above  
4. Put the PNG in `docs/diagrams/`  
5. Optional: also save the `.drawio` source next to it  
6. Commit and push  

```bash
git add docs/diagrams report media docs/phases
git commit -m "Add report, diagrams and phase documents for assessment"
git push origin main
```

## Caption template for Word

```
Figure 9 – UML Deployment Diagram (offline school workstation)

GitHub: https://github.com/katlehochristophersenqhe-maker/CS4433-LMS/blob/main/docs/diagrams/09_deployment.png
```

