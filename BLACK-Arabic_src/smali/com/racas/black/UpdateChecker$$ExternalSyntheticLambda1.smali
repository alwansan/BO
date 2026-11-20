.class public final synthetic Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Ljava/lang/Runnable;


# instance fields
.field public final synthetic f$0:Lcom/racas/black/UpdateChecker$Listener;

.field public final synthetic f$1:Lcom/racas/black/UpdateChecker$Release;

.field public final synthetic f$2:Z


# direct methods
.method public synthetic constructor <init>(Lcom/racas/black/UpdateChecker$Listener;Lcom/racas/black/UpdateChecker$Release;Z)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$0:Lcom/racas/black/UpdateChecker$Listener;

    iput-object p2, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$1:Lcom/racas/black/UpdateChecker$Release;

    iput-boolean p3, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$2:Z

    return-void
.end method


# virtual methods
.method public final run()V
    .locals 2

    .line 0
    iget-object v0, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$0:Lcom/racas/black/UpdateChecker$Listener;

    iget-object v1, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$1:Lcom/racas/black/UpdateChecker$Release;

    iget-boolean p0, p0, Lcom/racas/black/UpdateChecker$$ExternalSyntheticLambda1;->f$2:Z

    invoke-static {v0, v1, p0}, Lcom/racas/black/UpdateChecker;->lambda$check$0(Lcom/racas/black/UpdateChecker$Listener;Lcom/racas/black/UpdateChecker$Release;Z)V

    return-void
.end method
